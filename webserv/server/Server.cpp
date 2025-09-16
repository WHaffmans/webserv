#include <arpa/inet.h> // For inet_addr
#include <cstdlib>     // For exit()
#include <cstring>     // For memset
#include <fcntl.h>     // For fcntl()"
#include <iostream>
#include <netinet/in.h> // For sockaddr_in
#include <sys/epoll.h>
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For close()
#include <webserv/server/Server.hpp>

Server::Server(const ConfigManager &configManager) : epoll_fd(-1)
{
    const auto &serverConfigs = configManager.getServerConfigs();
    if (serverConfigs.empty())
    {
        throw std::runtime_error("No server configurations available.");
    }

	// for(auto serverConfig : serverConfigs) {
	// 	setupServerSocket(serverConfig);
	// }
    // For simplicity, just take the first server config
    const ServerConfig &config = serverConfigs[0];
    host = config.getHost();
    port = config.getPort();
    std::cout << "Server initialized on " << host << ":" << port << '\n';
}


void Server::start()
{
    int server_fd = -1;
    

    // 1. Create socket (IPv4, TCP)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Set socket options (to reuse address and port)
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Bind socket to an IP/Port
    struct sockaddr_in address{};
    // std::memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    address.sin_port = htons(port);       // Host-to-network byte order

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) // NOLINT
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 4. Listen for incoming connections
    if (listen(server_fd, SOMAXCONN) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    std::cout << "Server listening on port " << port << "...\n";

    // 5. Set up epoll
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll_create1 failed");
        close(server_fd);
        // throw exception
    }

    struct epoll_event event;
    event.events = EPOLLIN; // Interested in read events
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
    {
        perror("epoll_ctl failed");
        close(server_fd);
        close(epoll_fd);
        // throw exception
    }
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS]; // NOLINT
    while (true)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1); // NOLINT
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
            if ((event.events & EPOLLIN) > 0 && event.data.fd == server_fd)
            {
                handleConnection(epoll_fd, &event);
            }
            else if ((event.events & EPOLLIN) > 0)
            {
                handleRequest(epoll_fd, &event);
            } else if ((event.events & EPOLLOUT) > 0) {
				std::cout << "Attempting to send data to fd: " << event.data.fd << '\n';
				const char *httpResponse = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
				ssize_t bytesSent = send(event.data.fd, httpResponse, strlen(httpResponse), 0);
				if (bytesSent < 0)
				{
					perror("Send error");
				}
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

    struct epoll_event ev;
    ev.events = EPOLLOUT | EPOLLET;
    ev.data.fd = client_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
}

void Server::handleConnection(int epoll_fd, struct epoll_event *event)
{
    int client_fd = accept(event->data.fd, nullptr, nullptr);
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
    // close(client_fd); // Close the client socket for now
}