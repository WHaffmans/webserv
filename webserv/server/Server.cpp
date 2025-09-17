#include <algorithm>   // For std::find
#include <arpa/inet.h> // For inet_addr
#include <cstdlib>     // For exit()
#include <cstring>     // For memset
#include <fcntl.h>     // For fcntl()"
#include <iostream>
#include <map>
#include <netinet/in.h> // For sockaddr_in
#include <ranges>       // For std::ranges::find
#include <sys/epoll.h>
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For close()
#include <vector>
#include <webserv/server/Server.hpp>

Server::Server(const ConfigManager &configManager) : _epoll_fd(-1), _configManager(configManager)
{
    const auto &serverConfigs = configManager.getServerConfigs();
    if (serverConfigs.empty())
    {
        throw std::runtime_error("No server configurations available.");
    }
}

int createServerSocket(const std::string &host, int port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host.c_str());
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) // NOLINT
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, SOMAXCONN) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    std::cout << "Server listening on " << host << ":" << port << "...\n";
    return server_fd;
}
void Server::start()
{
    std::cout << "Starting servers...\n";
    // 1. Load server configurations

    for (const auto &config : _configManager.get().getServerConfigs())
    {
        int server_fd = createServerSocket(config.getHost(), config.getPort());
        _fdToConfig.insert({server_fd, std::cref(config)});
    }
    if (_fdToConfig.empty())
    {
        throw std::runtime_error("No server sockets created.");
    }

    // 5. Set up epoll
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll_create1 failed");
        // TODO: Close all server FDs
        //  for (int server_fd : _server_fds)
        //  {
        //      close(server_fd);
        //  }
    }

    for (const auto &pair : _fdToConfig)
    {
        int server_fd = pair.first;
        struct epoll_event event{};
        event.events = EPOLLIN; // Interested in read events
        event.data.fd = server_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
        {
            perror("epoll_ctl failed");
            // TODO close fds
            close(epoll_fd);
            // throw exception
        }
        // _server_fds.push_back(server_fd);
    }
    _epoll_fd = epoll_fd;
    eventLoop();
}

void Server::eventLoop()
{
    // Placeholder for the event loop logic
    std::cout << "Entering event loop...\n";
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS]; // NOLINT
    while (true)
    {
        int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1); // NOLINT
        if (nfds == -1)
        {
            perror("epoll_wait failed");
            break;
        }
        for (int i = 0; i < nfds; ++i)
        {
            epoll_event &event = events[i];
            if ((event.events & EPOLLERR) > 0 || (event.events & EPOLLHUP) > 0)
            {
                std::cerr << "Epoll error on fd " << event.data.fd << '\n';
                close(event.data.fd);
            }
            else if (_fdToConfig.contains(event.data.fd))
            {
                handleConnection(_epoll_fd, &event);
            }
            else if ((event.events & EPOLLIN) > 0)
            {
                handleRequest(_epoll_fd, &event);
            }
            else if ((event.events & EPOLLOUT) > 0)
            {
                std::cout << "Attempting to send data to fd: " << event.data.fd << '\n';
                std::string response = clients.at(event.data.fd).getResponse();
                const char *httpResponse = response.c_str();
                ssize_t bytesSent = send(event.data.fd, httpResponse, strlen(httpResponse), 0);
                if (bytesSent < 0)
                {
                    perror("Send error");
                }
                clients.erase(event.data.fd);
                close(event.data.fd); // Close after sending response
            }
        }
    }
}

void Server::handleRequest(int epoll_fd, struct epoll_event *event)
{
    std::cout << "Handling request...\n";

    int client_fd = event->data.fd;
    std::cout << "client fd: " << client_fd << '\n';
    char buffer[1024] = {};
    ssize_t bytesRead = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytesRead < 0)
    {
        perror("Read error");
        close(client_fd);
        return;
    }
    if (bytesRead == 0)
    {
        std::cout << "Client disconnected, fd: " << client_fd << '\n';
        close(client_fd);
        return;
    }

    buffer[bytesRead] = '\0'; // Null-terminate the buffer
    std::cout << "Received request:\n" << buffer << '\n';

    // clients[client_fd] = Client(client_fd, *this, ServerConfig);
    clients.at(client_fd).request(buffer);
}

void Server::responseReady(int client_fd)
{
    std::cout << "Response ready for client fd: " << client_fd << '\n';
    struct epoll_event ev;
    ev.events = EPOLLOUT | EPOLLET;
    ev.data.fd = client_fd;
    epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
}

void Server::handleConnection(int epoll_fd, struct epoll_event *event)
{
    int client_fd = accept(event->data.fd, nullptr, nullptr);
    std::cout << "connection " << client_fd << '\n';
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    if (client_fd == -1)
    {
        perror("Accept failed");
        return;
    }
    std::cout << "New connection accepted, fd: " << client_fd << '\n';
    struct epoll_event client_event;
    client_event.events = EPOLLIN | EPOLLET;
    client_event.data.fd = client_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
    {
        perror("epoll_ctl failed");
        close(client_fd);
        close(epoll_fd);
        // throw exception
    }

    std::cout << "client fd " << client_fd << ", server fd " << event->data.fd << '\n';
    clients.insert({client_fd, Client(client_fd, *this, _fdToConfig.at(event->data.fd))});
}