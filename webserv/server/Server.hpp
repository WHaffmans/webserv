#pragma once

#include "webserv/config/ServerConfig.hpp"
#include <functional>
#include <memory>
#include <unordered_map>
#include <webserv/client/Client.hpp>
#include <webserv/config/ConfigManager.hpp>
#include <webserv/socket/Socket.hpp>

class Client;

class Server
{
  public:
    Server() = delete;
    Server(const ConfigManager &configManager);

    Server(const Server &other) = delete;                // Disable copy constructor
    Server &operator=(const Server &other) = delete;     // Disable copy assignment
    Server(Server &&other) noexcept = delete;            // Move constructor
    Server &operator=(Server &&other) noexcept = delete; // Move assignment

    // The constructor must initialize the reference member 'configManager'
    // Implementation should be in the .cpp file using an initializer list

    ~Server();

    void start();
    void addToEpoll(const Socket &socket, uint32_t events) const;
    void removeFromEpoll(const Socket &socket) const ;
    void setupServerSocket(const ServerConfig &config);
    void handleConnection(struct epoll_event *event);
    void handleRequest(struct epoll_event *event) const;
    void responseReady(int client_fd) const;
    void eventLoop();
    Socket &getListener(int fd) const;
    Client &getClient(int fd) const;
    const ServerConfig &getConfig(int fd) const;
    const ServerConfig &getConfig(const Socket &socket) const;


  private:
    int epoll_fd_;
    const ConfigManager &configManager_;
    std::vector<std::unique_ptr<Socket>> listeners_;
    std::unordered_map<int, std::reference_wrapper<const ServerConfig>> fdToConfig_;
    std::unordered_map<int, std::unique_ptr<Client>> clients_;
};
