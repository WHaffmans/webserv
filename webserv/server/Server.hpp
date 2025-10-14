#pragma once

#include <webserv/client/Client.hpp>
#include <webserv/config/ConfigManager.hpp>
#include <webserv/config/ServerConfig.hpp> // for ServerConfig
#include <webserv/router/Router.hpp>       // for Router
#include <webserv/socket/ASocket.hpp>
#include <webserv/socket/ServerSocket.hpp> // for ServerSocket

#include <cstdint>       // for uint32_t
#include <memory>        // for unique_ptr
#include <set>           // for set
#include <unordered_map> // for unordered_map
#include <vector>        // for vector

class Client;
class ConfigManager;
class ServerConfig;
class ASocket;
class ServerSocket;

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

    void run();
    void add(const ASocket &socket, uint32_t events, Client *client = nullptr);
    void remove(const ASocket &socket);
    void disconnect(const Client &client);
    void responseReady(int client_fd) const;

    ServerSocket &getListener(int fd) const;
    Client &getClient(int fd) const;
    const Router &getRouter() const;

  private:
    int epoll_fd_;
    const ConfigManager &configManager_;
    const Router router_;
    std::vector<std::unique_ptr<ServerSocket>> listeners_;
    std::set<int> listener_fds_;
    // std::unordered_map<int, std::unique_ptr<Client>> clients_;
    std::vector<std::unique_ptr<Client>> clients_;
    std::unordered_map<int, Client *> socketToClient_;

    void handleEvent(struct epoll_event *event);
    void handleConnection(struct epoll_event *event);
    void handleRequest(struct epoll_event *event) const;
    void handleResponse(struct epoll_event *event);

    void setupServerSocket(const ServerConfig &config);
};
