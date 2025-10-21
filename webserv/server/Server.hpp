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

    Server(const Server &other) = delete;
    Server &operator=(const Server &other) = delete;
    Server(Server &&other) noexcept = delete;
    Server &operator=(Server &&other) noexcept = delete;

    ~Server();

    void run();
    void add(ASocket &socket, Client *client = nullptr);
    void remove(ASocket &socket);
    void update(const ASocket &socket) const;
    void disconnect(const Client &client);
    void writable(int client_fd) const;

    ServerSocket &getListener(int fd) const;
    Client &getClient(int fd) const;

  private:
    int epoll_fd_;
    const ConfigManager &configManager_;
    std::vector<std::unique_ptr<ServerSocket>> listeners_;
    std::set<int> listener_fds_;
    std::vector<std::unique_ptr<Client>> clients_;
    std::unordered_map<int, Client *> socketToClient_;
    std::set<ASocket *> sockets_;

    void pollClients() const;
    void pollSockets();
    void handleEpoll(struct epoll_event *events, int max_events);

    void handleEpollHangUp(struct epoll_event *event) const;
    void handleEvent(struct epoll_event *event);
    void handleConnection(struct epoll_event *event);
    void handleRequest(struct epoll_event *event) const;
    void handleResponse(struct epoll_event *event) const;

    void setupServerSocket(const ServerConfig &config);
};
