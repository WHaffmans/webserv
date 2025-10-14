#include <webserv/client/Client.hpp>        // for Client
#include <webserv/config/ConfigManager.hpp> // for ConfigManager
#include <webserv/config/ServerConfig.hpp>  // for ServerConfig
#include <webserv/log/Log.hpp>              // for Log, LOCATION
#include <webserv/server/Server.hpp>
#include <webserv/socket/Socket.hpp> // for Socket

#include <cerrno>        // for errno
#include <cstring>       // for strerror
#include <exception>     // for exception
#include <memory>        // for unique_ptr, allocator, make_unique
#include <optional>      // for optional
#include <stdexcept>     // for runtime_error
#include <string>        // for basic_string, operator+, to_string, char_traits, string
#include <unordered_map> // for unordered_map, operator==
#include <utility>       // for move, pair
#include <vector>        // for vector

#include <sys/epoll.h> // for epoll_event, epoll_ctl, EPOLLIN, EPOLLOUT, epoll_create1, epoll_wait, EPOLLERR, EPOLLHUP, EPOLL_CTL_ADD, EPOLL_CTL_DEL, EPOLL_CTL_MOD
#include <sys/socket.h> // for send, SOMAXCONN
#include <sys/types.h>  // for ssize_t
#include <unistd.h>     // for close

class Router;

Server::Server(const ConfigManager &configManager)
    : epoll_fd_(epoll_create1(0)), configManager_(configManager), router_()
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
}

Server::~Server()
{
    Log::trace(LOCATION);
    if (epoll_fd_ != -1)
    {
        close(epoll_fd_);
    }
}

void Server::start()
{
    Log::trace(LOCATION);
    Log::info("Starting servers...");
    // 1. Load server configurations

    for (const auto &config : configManager_.getServerConfigs())
    {
        setupServerSocket(*config);
    }
    if (listener_fds_.empty())
    {
        Log::fatal("No server sockets created.");
        throw std::runtime_error("No server sockets created.");
    }

    eventLoop();
}

void Server::add(const Socket &socket, uint32_t events) const
{
    Log::trace(LOCATION);
    int fd = socket.getFd();
    struct epoll_event event{};
    event.events = events;
    event.data.fd = fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        Log::error("epoll_ctl ADD failed for fd: " + std::to_string(fd));
        throw std::runtime_error("epoll_ctl ADD failed");
    }
}

void Server::disconnect(const Client &client)
{
    Log::trace(LOCATION);
    int client_fd = client.getSocket().getFd();
    clients_.erase(client_fd);
}

void Server::remove(const Socket &socket) const
{
    Log::trace(LOCATION);
    int filedes = socket.getFd();
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, filedes, nullptr) == -1)
    {
        Log::error("epoll_ctl DEL failed for fd: " + std::to_string(filedes));
        throw std::runtime_error("epoll_ctl DEL failed");
    }
}

void Server::setupServerSocket(const ServerConfig &config)
{
    Log::trace(LOCATION);
    try
    {
        auto host = config.get<std::string>("host").value_or(std::string()); // TODO should not be a default host

        auto port = config.get<int>("listen").value_or(0); // TODO should not be a default port
        std::unique_ptr<Socket> serverSocket = std::make_unique<Socket>();
        serverSocket->bind(host, port);
        serverSocket->listen(SOMAXCONN);
        int server_fd = serverSocket->getFd();

        add(*serverSocket, EPOLLIN);

        listeners_.push_back(std::move(serverSocket));
        listener_fds_.insert(server_fd);
        Log::info("Server listening on " + host + ":" + std::to_string(port) + "...");
        // static_cast<std::string>(config["listen"]) + "...");
    }
    catch (const std::exception &e)
    {
        Log::error("Error setting up server socket: " + std::string(e.what()));
    }
}

void Server::handleConnection(struct epoll_event *event)
{
    Log::trace(LOCATION);
    Socket &listener = getListener(event->data.fd);
    std::unique_ptr<Socket> clientSocket = listener.accept();
    add(*clientSocket, EPOLLIN);
    clients_.insert({clientSocket->getFd(), std::make_unique<Client>(std::move(clientSocket), *this)});
}

Socket &Server::getListener(int fd) const
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
    Log::trace(LOCATION);
    auto it = clients_.find(fd);
    if (it != clients_.end())
    {
        return *(it->second);
    }
    Log::error("Client not found for fd: " + std::to_string(fd));
    throw std::runtime_error("Client not found for fd: " + std::to_string(fd));
}

void Server::handleRequest(struct epoll_event *event) const
{
    Log::trace(LOCATION);
    int client_fd = event->data.fd;

    Client &client = getClient(client_fd);
    client.request();
}

void Server::responseReady(int client_fd) const
{
    Log::trace(LOCATION);
    Log::debug("Response ready for client fd: " + std::to_string(client_fd));
    struct epoll_event ev{};
    ev.events = EPOLLOUT;
    ev.data.fd = client_fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, client_fd, &ev) == -1)
    {
        Log::error("epoll_ctl MOD failed for fd: " + std::to_string(client_fd));
        throw std::runtime_error("epoll_ctl MOD failed");
    }
}

void Server::handleResponse(struct epoll_event *event)
{
    Log::debug("Attempting to send data to fd: " + std::to_string(event->data.fd));
    Client &client = getClient(event->data.fd);
    auto httpResponse = client.getResponse();
    ssize_t bytesSent = send(event->data.fd, httpResponse.data(), httpResponse.size(), 0);
    if (bytesSent < 0)
    {
        Log::error("Send failed for fd: " + std::to_string(event->data.fd) + " with error: " + std::strerror(errno));
    }
    else
    {
        Log::debug("Sent " + std::to_string(bytesSent) + " bytes to fd: " + std::to_string(event->data.fd));
    }
    clients_.erase(event->data.fd);
}

void Server::handleEvent(struct epoll_event *event)
{
    Log::trace(LOCATION);
    if ((event->events & EPOLLERR) > 0 || (event->events & EPOLLHUP) > 0)
    {
        Log::error("Epoll error on fd " + std::to_string(event->data.fd));
        remove(getListener(event->data.fd));
        close(event->data.fd);
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

void Server::eventLoop()
{
    Log::trace(LOCATION);
    Log::info("Listening...");
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS]; // NOLINT
    while (true)
    {
        int nfds = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1); // NOLINT
        if (nfds == -1)
        {
            Log::error("epoll_wait failed");
            throw std::runtime_error("epoll_wait failed");
        }
        for (int i = 0; i < nfds; ++i)
        {
            handleEvent(&events[i]); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        }
    }
}

const Router &Server::getRouter() const
{
    return router_;
}