#include "webserv/socket/Socket.hpp"
#include <arpa/inet.h> // For inet_addr
#include <cstdlib>     // For exit()
#include <cstring>     // For memset
#include <fcntl.h>     // For fcntl()"
#include <iostream>
#include <memory>
#include <netinet/in.h> // For sockaddr_in
#include <sys/epoll.h>
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For close()
#include <vector>
#include <webserv/server/Server.hpp>

Server::Server(const ConfigManager &configManager) : epoll_fd_(epoll_create1(0)), configManager_(configManager)
{
    const auto &serverConfigs = configManager.getServerConfigs();
    if (serverConfigs.empty())
    {
        throw std::runtime_error("No server configurations available.");
    }
    if (epoll_fd_ == -1)
    {
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
    std::cout << "Starting servers...\n";
    // 1. Load server configurations

    for (const auto &config : configManager_.getServerConfigs())
    {
        setupServerSocket(config);
    }
    if (fdToConfig_.empty())
    {
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
        throw std::runtime_error("epoll_ctl ADD failed");
    }
}

void Server::removeFromEpoll(const Socket &socket) const
{
    int filedes = socket.getFd();
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, filedes, nullptr) == -1)
    {
        throw std::runtime_error("epoll_ctl DEL failed");
    }
}

void Server::setupServerSocket(const ServerConfig &config)
{
    try
    {
        std::unique_ptr<Socket> serverSocket = std::make_unique<Socket>();
        serverSocket->bind(config.getHost(), config.getPort());
        serverSocket->listen(SOMAXCONN);
        int server_fd = serverSocket->getFd();

        addToEpoll(*serverSocket, EPOLLIN);

        listeners_.push_back(std::move(serverSocket));
        fdToConfig_.insert({server_fd, std::cref(config)});
        std::cout << "Server listening on " << config.getHost() << ":" << config.getPort() << "...\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error setting up server socket: " << e.what() << '\n';
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
    throw std::runtime_error("Listener not found for fd: " + std::to_string(fd));
}

Client &Server::getClient(int fd) const
{
    auto it = clients_.find(fd);
    if (it != clients_.end())
    {
        return *(it->second);
    }
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
    throw std::runtime_error("Config not found for fd: " + std::to_string(fd));
}

void Server::handleRequest(struct epoll_event *event)
{
    int client_fd = event->data.fd;

    Client &client = getClient(client_fd);
    client.request();
}

void Server::responseReady(int client_fd) const
{
    std::cout << "Response ready for client fd: " << client_fd << '\n';
    struct epoll_event ev{};
    ev.events = EPOLLOUT;
    ev.data.fd = client_fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, client_fd, &ev) == -1)
    {
        throw std::runtime_error("epoll_ctl MOD failed");
    }
}

void Server::eventLoop()
{
    std::cout << "Entering event loop...\n";
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS]; // NOLINT
    while (true)
    {
        int nfds = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1); // NOLINT
        if (nfds == -1)
        {
            throw std::runtime_error("epoll_wait failed");
        }
        for (int i = 0; i < nfds; ++i)
        {
            epoll_event &event = events[i];
            if ((event.events & EPOLLERR) > 0 || (event.events & EPOLLHUP) > 0)
            {
                std::cerr << "Epoll error on fd " << event.data.fd << '\n';
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
                std::cout << "Attempting to send data to fd: " << event.data.fd << '\n';
                Client &client = getClient(event.data.fd);
                std::string response = client.getResponse();
                const char *httpResponse = response.c_str();
                ssize_t bytesSent = send(event.data.fd, httpResponse, strlen(httpResponse), 0);
                if (bytesSent < 0)
                {
                    perror("Send error");
                }
                clients_.erase(event.data.fd);
            }
        }
    }
}
