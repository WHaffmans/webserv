#pragma once

#include <webserv/client/Client.hpp>
#include <webserv/config/ConfigManager.hpp>
#include <webserv/config/ServerConfig.hpp> // for ServerConfig
#include <webserv/router/Router.hpp>       // for Router
#include <webserv/socket/Socket.hpp>       // for Socket

#include <cstdint>       // for uint32_t
#include <memory>        // for unique_ptr
#include <set>           // for set
#include <unordered_map> // for unordered_map
#include <vector>        // for vector

class Client;
class ConfigManager;
class ServerConfig;

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
    void add(const Socket &socket, uint32_t events) const;
    void remove(const Socket &socket) const;
    void disconnect(const Client &client);
    void responseReady(int client_fd) const;

    Socket &getListener(int fd) const;
    Client &getClient(int fd) const;
    const Router &getRouter() const;

  private:
    int epoll_fd_;
    const ConfigManager &configManager_;
    const Router router_;
    std::vector<std::unique_ptr<Socket>> listeners_;
    std::set<int> listener_fds_;
    std::unordered_map<int, std::unique_ptr<Client>> clients_;

    void handleEvent(struct epoll_event *event);
    void handleConnection(struct epoll_event *event);
    void handleRequest(struct epoll_event *event) const;
    void handleResponse(struct epoll_event *event);

    void setupServerSocket(const ServerConfig &config);
    void eventLoop();
  };
