#pragma once

// #include "webserv/http/HttpResponse.hpp"

#include "webserv/http/HttpResponse.hpp"

#include <webserv/config/ServerConfig.hpp> // for ServerConfig
#include <webserv/http/HttpConstants.hpp>  // for OK
#include <webserv/http/HttpRequest.hpp>    // for HttpRequest
#include <webserv/server/Server.hpp>
#include <webserv/socket/Socket.hpp>

#include <cstddef> // for size_t
#include <cstdint>
#include <memory> // for unique_ptr
#include <vector>

class Server;
class Socket;
class ServerConfig;
class HttpResponse;

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
    [[nodiscard]] std::vector<uint8_t> getResponse() const;

    [[nodiscard]] bool isResponseReady() const;
    [[nodiscard]] int getStatusCode() const;

    [[nodiscard]] Socket &getSocket() const { return *client_socket_; }

    void setStatusCode(int code);

  private:
    int statusCode_ = Http::StatusCode::OK;
    constexpr static size_t bufferSize_ = 4096;
    std::unique_ptr<HttpRequest> httpRequest_ = nullptr;
    std::unique_ptr<HttpResponse> httpResponse_ = nullptr;
    std::unique_ptr<Socket> client_socket_;
    Server &server_;
    mutable const ServerConfig *server_config_ = nullptr;
};