#pragma once

#include "webserv/socket/Socket.hpp"

#include <webserv/config/ServerConfig.hpp>
#include <webserv/server/Server.hpp>

#include <memory>

class Server;

class Client
{
  public:
    Client(std::unique_ptr<Socket> socket, Server &server, const ServerConfig &server_config);

    Client(const Client &other) = delete;                // Disable copy constructor
    Client &operator=(const Client &other) = delete;     // Disable copy assignment
    Client(Client &&other) noexcept = delete;            // Move constructor
    Client &operator=(Client &&other) noexcept = delete; // Move assignment

    ~Client();

    void request();
    [[nodiscard]] std::string getResponse() const;

  private:
    int parseHeaderforContentLength(const std::string &request);
    int contentLength_{-1};
    std::string requestBuffer_;
    std::string header_;
    std::string content_;
    std::unique_ptr<Socket> client_socket_;
    const Server &server_;
    const ServerConfig &server_config_;
};