#pragma once

#include <webserv/config/ServerConfig.hpp> // for ServerConfig
#include <webserv/http/HttpConstants.hpp>  // for StatusCode, HTTP
#include <webserv/http/HttpRequest.hpp>    // for HttpRequest
#include <webserv/server/Server.hpp>
#include <webserv/socket/Socket.hpp>

#include <cstddef> // for size_t
#include <memory>  // for unique_ptr
#include <string>  // for string

class Server;
class Socket;
class ServerConfig;

class Client
{
  public:
    Client(std::unique_ptr<Socket> socket, Server &server);

    Client(const Client &other) = delete;                // Disable copy constructor
    Client &operator=(const Client &other) = delete;     // Disable copy assignment
    Client(Client &&other) noexcept = delete;            // Move constructor
    Client &operator=(Client &&other) noexcept = delete; // Move assignment

    ~Client();

    void request();
    [[nodiscard]] std::string getResponse() const;

    [[nodiscard]] int getStatusCode() const;
    void setStatusCode(int code);

  private:
    int statusCode_ = Http::StatusCode::OK;
    constexpr static size_t bufferSize_ = 4096;
    std::unique_ptr<HttpRequest> httpRequest_ = nullptr;
    std::unique_ptr<Socket> client_socket_;
    Server &server_;
    mutable const ServerConfig *server_config_ = nullptr;
};