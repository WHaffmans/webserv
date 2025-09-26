#include <webserv/client/Client.hpp>        // for Client
#include <webserv/config/ConfigManager.hpp> // for ConfigManager
#include <webserv/config/ServerConfig.hpp>  // for ServerConfig
#include <webserv/log/Log.hpp>              // for Log
#include <webserv/server/Server.hpp>
#include <webserv/socket/Socket.hpp> // for Socket

#include <cerrno>    // for errno
#include <cstring>   // for strerror, strlen
#include <exception> // for exception
#include <memory>    // for unique_ptr, allocator, make_unique
#include <stdexcept> // for runtime_error
#include <string>    // for basic_string, operator+, to_string, char_traits, string
#include <utility>   // for move, pair
#include <vector>    // for vector

#include <sys/epoll.h> // for epoll_event, epoll_ctl, EPOLLIN, EPOLLOUT, epoll_create1, epoll_wait, EPOLLERR, EPOLLHUP, EPOLL_CTL_ADD, EPOLL_CTL_DEL, EPOLL_CTL_MOD
#include <sys/socket.h> // for send, SOMAXCONN
#include <sys/types.h>  // for ssize_t
#include <unistd.h>     // for close

Server::Server(const ConfigManager &configManager) : epoll_fd_(epoll_create1(0)), configManager_(configManager)
{
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
    if (epoll_fd_ != -1)
    {
        close(epoll_fd_);
    }
}

void Server::start()
{
    Log::info("Starting servers...");
    // 1. Load server configurations

    for (const auto &config : configManager_.getServerConfigs())
    {
        setupServerSocket(*config);
    }
    if (fdToConfig_.empty())
    {
        Log::fatal("No server sockets created.");
        throw std::runtime_error("No server sockets created.");
    }

    eventLoop();
}

void Server::addToEpoll(const Socket &socket, uint32_t events) const
{
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

void Server::removeFromEpoll(const Socket &socket) const
{
    int filedes = socket.getFd();
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, filedes, nullptr) == -1)
    {
        Log::error("epoll_ctl DEL failed for fd: " + std::to_string(filedes));
        throw std::runtime_error("epoll_ctl DEL failed");
    }
}

void Server::setupServerSocket(const ServerConfig &config)
{
    try
    {
        auto host = config.getDirectiveValue<std::string>("host");
        auto port = config.getDirectiveValue<int>("listen");
        std::unique_ptr<Socket> serverSocket = std::make_unique<Socket>();
        serverSocket->bind(host, port);
        serverSocket->listen(SOMAXCONN);
        int server_fd = serverSocket->getFd();

        addToEpoll(*serverSocket, EPOLLIN);

        listeners_.push_back(std::move(serverSocket));
        fdToConfig_.insert({server_fd, std::cref(config)});
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
    Socket &listener = getListener(event->data.fd);
    std::unique_ptr<Socket> clientSocket = listener.accept();
    addToEpoll(*clientSocket, EPOLLIN);
    clients_.insert(
        {clientSocket->getFd(), std::make_unique<Client>(std::move(clientSocket), *this, getConfig(listener))});
}

Socket &Server::getListener(int fd) const
{
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
    auto it = clients_.find(fd);
    if (it != clients_.end())
    {
        return *(it->second);
    }
    Log::error("Client not found for fd: " + std::to_string(fd));
    throw std::runtime_error("Client not found for fd: " + std::to_string(fd));
}

const ServerConfig &Server::getConfig(const Socket &socket) const
{
    return getConfig(socket.getFd());
}

const ServerConfig &Server::getConfig(int fd) const
{
    auto it = fdToConfig_.find(fd);
    if (it != fdToConfig_.end())
    {
        return (it->second.get());
    }
    Log::error("Config not found for fd: " + std::to_string(fd));
    throw std::runtime_error("Config not found for fd: " + std::to_string(fd));
}

void Server::handleRequest(struct epoll_event *event) const
{
    int client_fd = event->data.fd;

    Client &client = getClient(client_fd);
    client.request();
}

void Server::responseReady(int client_fd) const
{
    Log::info("Response ready for client fd: " + std::to_string(client_fd));
    struct epoll_event ev{};
    ev.events = EPOLLOUT;
    ev.data.fd = client_fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, client_fd, &ev) == -1)
    {
        Log::error("epoll_ctl MOD failed for fd: " + std::to_string(client_fd));
        throw std::runtime_error("epoll_ctl MOD failed");
    }
}

void Server::eventLoop()
{
    Log::info("Entering event loop...");
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
            epoll_event &event = events[i]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
            if ((event.events & EPOLLERR) > 0 || (event.events & EPOLLHUP) > 0)
            {
                Log::error("Epoll error on fd " + std::to_string(event.data.fd));
                removeFromEpoll(getListener(event.data.fd));
                close(event.data.fd);
            }
            else if (fdToConfig_.contains(event.data.fd))
            {
                handleConnection(&event);
            }
            else if ((event.events & EPOLLIN) > 0)
            {
                handleRequest(&event);
            }
            else if ((event.events & EPOLLOUT) > 0)
            {
                Log::info("Attempting to send data to fd: " + std::to_string(event.data.fd));
                Client &client = getClient(event.data.fd);
                std::string response = client.getResponse();
                const char *httpResponse = response.c_str();
                ssize_t bytesSent = send(event.data.fd, httpResponse, strlen(httpResponse), 0);
                if (bytesSent < 0)
                {
                    Log::error("Send failed for fd: " + std::to_string(event.data.fd) +
                               " with error: " + std::strerror(errno));
                }
                else
                {
                    Log::info("Sent " + std::to_string(bytesSent) + " bytes to fd: " + std::to_string(event.data.fd));
                }
                clients_.erase(event.data.fd);
            }
        }
    }
}
