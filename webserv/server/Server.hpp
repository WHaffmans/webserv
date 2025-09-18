#pragma once

#include <functional>
#include <webserv/config/ConfigManager.hpp>
#include <webserv/client/Client.hpp>

class Client;

class Server
{
  public:
    Server(const ConfigManager &configManager);

    // The constructor must initialize the reference member 'configManager'
    // Implementation should be in the .cpp file using an initializer list

    void start();
    void setupServerSocket(ServerConfig &config);
    void handleConnection(int epoll_fd, struct epoll_event *event);
    void handleRequest(int epoll_fd, struct epoll_event *event);
    void responseReady(int client_fd);
    void eventLoop();

  private:
    int _epoll_fd;
    std::reference_wrapper<const ConfigManager> _configManager;
    std::map<int, std::reference_wrapper<const ServerConfig>> _fdToConfig;
    std::vector<int> _server_fds;
    std::map<int, Client> clients;
};
