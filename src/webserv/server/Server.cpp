#include <webserv/server/Server.hpp>

#include <webserv/client/Client.hpp>        // for Client
#include <webserv/config/ConfigManager.hpp> // for ConfigManager
#include <webserv/config/ServerConfig.hpp>  // for ServerConfig
#include <webserv/log/Log.hpp>              // for Log, LOCATION
#include <webserv/socket/ASocket.hpp>      // for ASocket
#include <webserv/socket/ClientSocket.hpp> // for ClientSocket
#include <webserv/socket/ServerSocket.hpp> // for ServerSocket
#include <webserv/utils/utils.hpp>         // for stateToEpoll

#include <cerrno>  // for errno, EBADF, ENOENT, EINTR
#include <csignal> // for SIGINT, sig_atomic_t
#include <cstdio>
#include <cstring>       // for strerror
#include <exception>     // for exception
#include <memory>        // for unique_ptr, allocator, make_unique
#include <optional>      // for optional
#include <stdexcept>     // for runtime_error, invalid_argument
#include <string>        // for basic_string, operator+, to_string, char_traits, string
#include <unordered_map> // for unordered_map
#include <utility>       // for move
#include <vector>        // for vector, erase_if

#include <fcntl.h>     // for O_CLOEXEC
#include <stdint.h>    // for uint32_t
#include <sys/epoll.h> // for epoll_event, epoll_ctl, EPOLLOUT, EPOLL_CTL_MOD, epoll_create1, epoll_wait, EPOLLERR, EPOLLHUP, EPOLLIN, EPOLL_CTL_ADD, EPOLL_CTL_DEL
#include <sys/socket.h> // for SOMAXCONN
#include <unistd.h>     // for close

volatile sig_atomic_t Server::signum_ = 0;

Server::Server(const ConfigManager &configManager) : epoll_fd_(epoll_create1(O_CLOEXEC)), configManager_(configManager)
{
    Log::trace(LOCATION);
    const auto &serverConfigs = configManager.getServerConfigs();
    if (serverConfigs.empty())
    {
        Log::fatal("No server configurations available.");
        throw std::runtime_error("No server configurations available.");
    }
    if (epoll_fd_ == -1)
    {
        Log::fatal("epoll_create1 failed");
        throw std::runtime_error("epoll_create1 failed");
    }
    for (const auto &config : configManager_.getServerConfigs())
    {
        setupServerSocket(*config);
    }
    if (listener_fds_.empty())
    {
        Log::fatal("No server sockets created.");
        throw std::runtime_error("No server sockets created.");
    }
}

Server::~Server()
{
    Log::trace(LOCATION);
    if (epoll_fd_ != -1)
    {
        close(epoll_fd_);
    }
}

void Server::add(ASocket &socket, Client *client)
{
    if (socket.getType() != ASocket::Type::SERVER_SOCKET && client == nullptr)
    {
        Log::error("Client pointer must be provided for non-server sockets");
        throw std::invalid_argument("Client pointer must be provided for non-server sockets");
    }
    Log::trace(LOCATION);
    int fd = socket.getFd();
    struct epoll_event event{};
    event.events = utils::stateToEpoll(socket.getEvent());
    event.data.fd = fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        Log::error(socket.toString() + ": epoll_ctl ADD failed");
        throw std::runtime_error("epoll_ctl ADD failed");
    }
    Log::debug(socket.toString() + ": added to epoll");
    socketToClient_[fd] = client;
    sockets_.insert(&socket);
}

void Server::remove(ASocket &socket)
{
    Log::trace(LOCATION);
    int fd = socket.getFd();
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) == -1)
    {
        if (errno == EBADF || errno == ENOENT)
        {
            Log::debug(socket.toString() + " was already closed or removed from epoll");
            return;
        }
        Log::error(socket.toString() + ": epoll_ctl DEL failed");
        throw std::runtime_error("epoll_ctl DEL failed");
    }
    socketToClient_.erase(fd);
    sockets_.erase(&socket);
}

void Server::disconnect(const Client &client)
{
    Log::trace(LOCATION);
    int client_fd = client.getSocket().getFd();

    std::erase_if(clients_, [&](const std::unique_ptr<Client> &c) { return c->getSocket().getFd() == client_fd; });
}

void Server::setupServerSocket(const ServerConfig &config)
{
    Log::trace(LOCATION);
    try
    {
        // These are required fields
        auto host = config.get<std::string>("host").value();
        auto port = config.get<int>("listen").value();
        for (const auto &listener : listeners_)
        {
            if (listener->getPort() == port && listener->getHost() == host)
            {
                Log::info("Server socket for " + host + ":" + std::to_string(port)
                          + " already exists, skipping creation.");
                return;
            }
        }
        std::unique_ptr<ServerSocket> serverSocket = std::make_unique<ServerSocket>(host, port);
        int server_fd = serverSocket->getFd();

        add(*serverSocket);

        listeners_.push_back(std::move(serverSocket));
        listener_fds_.insert(server_fd);
        Log::info("Server listening on " + host + ":" + std::to_string(port) + "...");
    }
    catch (const std::exception &e)
    {
        Log::error("Error setting up server socket: " + std::string(e.what()));
    }
}

void Server::handleConnection(struct epoll_event *event)
{
    Log::trace(LOCATION);
    ServerSocket &listener = getListener(event->data.fd);
    std::unique_ptr<ClientSocket> clientSocket = listener.accept();
    clientSocket->setIOState(ASocket::IoState::READ);
    auto client = std::make_unique<Client>(std::move(clientSocket), *this);
    add(client->getSocket(), client.get());
    clients_.emplace_back(std::move(client));
}

ServerSocket &Server::getListener(int fd) const
{
    Log::trace(LOCATION);
    for (const auto &listener : listeners_)
    {
        if (listener->getFd() == fd)
        {
            return *listener;
        }
    }
    Log::error("Listener not found for fd: " + std::to_string(fd));
    throw std::runtime_error("Listener not found for fd: " + std::to_string(fd));
}

Client &Server::getClient(int fd) const
{
    if (socketToClient_.contains(fd))
    {
        return *(socketToClient_.at(fd));
    }
    Log::error("Client not found for fd: " + std::to_string(fd));
    throw std::runtime_error("Client not found for fd: " + std::to_string(fd));
}

void Server::handleRequest(struct epoll_event *event) const
{
    Log::trace(LOCATION);
    int client_fd = event->data.fd;

    Client &client = getClient(client_fd);
    client.getSocket(client_fd).callback();
}

void Server::writable(int client_fd) const
{
    Log::trace(LOCATION);
    Client &client = getClient(client_fd);
    ASocket &socket = client.getSocket(client_fd);
    Log::debug(socket.toString() + ": response ready");
    struct epoll_event ev{};
    ev.events = EPOLLOUT;
    ev.data.fd = client_fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, client_fd, &ev) == -1)
    {
        Log::error(socket.toString() + ": epoll_ctl MOD failed");
        throw std::runtime_error("epoll_ctl MOD failed");
    }
}

void Server::update(const ASocket &socket) const
{
    Log::trace(LOCATION);

    int socketFd = socket.getFd();
    uint32_t events = utils::stateToEpoll(socket.getEvent());
    Log::debug(socket.toString() + ": is being updated");
    struct epoll_event evt{};
    evt.events = events;
    evt.data.fd = socketFd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, socketFd, &evt) == -1)
    {
        if (errno == EBADF || errno == ENOENT)
        {
            Log::debug(socket.toString() + ": was already closed or removed from epoll");
            return;
        }
        Log::error(socket.toString() + ": epoll_ctl MOD failed");
        throw std::runtime_error("epoll_ctl MOD failed");
    }
}

void Server::handleResponse(struct epoll_event *event) const
{
    int socket_fd = event->data.fd;
    Client &client = getClient(socket_fd);
    ASocket &socket = client.getSocket(socket_fd);
    Log::debug(socket.toString() + ": attempting to send data");
    socket.callback();
    // disconnect(client);
}

void Server::handleEpollHangUp(struct epoll_event *event) const
{
    Client &client = getClient(event->data.fd);
    ASocket &socket = client.getSocket(event->data.fd);
    if (socket.getType() == ASocket::Type::CGI_SOCKET)
    {
        Log::info(socket.toString() + ": CGI socket hang up");
        socket.callback();
        return;
    }
    Log::warning(socket.toString() + ": Epoll hang up");
}

void Server::handleEpollError(struct epoll_event *event)
{
    int fd = event->data.fd;
    Log::error("Epoll error on fd " + std::to_string(fd));
    try
    {
        Client &client = getClient(fd);
        ASocket &socket = client.getSocket(fd);
        switch (socket.getType())
        {
        case ASocket::Type::CLIENT_SOCKET:
            Log::warning(socket.toString() + ": EPOLLERR disconnecting client");
            disconnect(client);
            break;
        case ASocket::Type::SERVER_SOCKET:
            Log::warning(socket.toString() + ": EPOLLERR removing server socket");
            remove(socket);
            close(fd);
            listener_fds_.erase(fd);
            break;
        case ASocket::Type::TIMER_SOCKET:
        case ASocket::Type::CGI_SOCKET:
        default:
            Log::warning(socket.toString() + ": EPOLLERR removing auxiliary socket");
            remove(socket);
            close(fd);
            break;
        }
    }
    catch (const std::exception &e)
    {
        Log::warning(std::string("EPOLLERR on unknown/non-tracked fd, closing: ") + e.what());
        close(fd);
    }
}

void Server::handleEvent(struct epoll_event *event)
{
    Log::trace(LOCATION);
    if ((event->events & EPOLLERR) > 0)
    {
        handleEpollError(event);
        // return;
    }
    else if ((event->events & EPOLLHUP) > 0)
    {
        handleEpollHangUp(event);
    }
    else if (listener_fds_.contains(event->data.fd))
    {
        handleConnection(event);
    }
    else if ((event->events & EPOLLIN) > 0)
    {
        handleRequest(event);
    }
    else if ((event->events & EPOLLOUT) > 0)
    {
        handleResponse(event);
    }
}

void Server::handleEpoll(struct epoll_event *events, int max_events)
{
    int nfds = epoll_wait(epoll_fd_, events, max_events, 10); // NOLINT
    if (nfds == -1)
    {
        if (errno == EINTR)
        {
            Log::debug("epoll_wait interrupted by signal, continuing...");
            return;
        }
        Log::error("epoll_wait failed");
        throw std::runtime_error("epoll_wait failed");
    }
    for (int i = 0; i < nfds; ++i)
    {
        handleEvent(&events[i]); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }
}

void Server::pollClients() const
{
    for (const auto &client : clients_)
    {
        client->poll();
    }
}

void Server::pollSockets()
{
    for (auto *socket : sockets_)
    {
        if (socket->isDirty())
        {
            update(*socket);
            socket->processed();
        }
    }
}

void Server::run()
{
    Log::trace(LOCATION);
    Log::info("Listening...");
    const int MAX_EVENTS = 1024;
    struct epoll_event events[MAX_EVENTS]; // NOLINT
    while (signum_ != SIGINT && signum_ != SIGTERM)
    {
        std::string status = "Active connections: " + std::to_string(clients_.size());
        Log::status(status);
        pollSockets();
        pollClients();
        handleEpoll(events, MAX_EVENTS); // NOLINT (cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }
    Log::info("Server stopping...");
}

void Server::signalHandler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM)
    {
        signum_ = signum;
    }
}