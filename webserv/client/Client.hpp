#pragma once

// #include <webserv/http/HttpResponse.hpp>

#include "webserv/router/Router.hpp"
#include "webserv/socket/CgiSocket.hpp"

#include <webserv/config/ServerConfig.hpp> // for ServerConfig
#include <webserv/http/HttpConstants.hpp>  // for OK
#include <webserv/http/HttpRequest.hpp>    // for HttpRequest
#include <webserv/http/HttpResponse.hpp>   // for HttpResponse
#include <webserv/server/Server.hpp>
#include <webserv/socket/ClientSocket.hpp> // for ClientSocket

#include <cstddef> // for size_t
#include <cstdint> // for uint8_t
#include <memory>  // for unique_ptr
#include <vector>  // for vector

class Server;
class ClientSocket;
class ServerConfig;
class HttpResponse;

class Client
{
  public:
    Client(std::unique_ptr<ClientSocket> socket, Server &server);

    Client(const Client &other) = delete;                // Disable copy constructor
    Client &operator=(const Client &other) = delete;     // Disable copy assignment
    Client(Client &&other) noexcept = delete;            // Move constructor
    Client &operator=(Client &&other) noexcept = delete; // Move assignment

    ~Client();

    void request();
    void poll() const;

    [[nodiscard]] std::vector<uint8_t> getResponse() const;
    [[nodiscard]] int getStatusCode() const;

    [[nodiscard]] ClientSocket &getSocket() const { return *client_socket_; }

    void setStatusCode(int code);
    void setCgiSocket(CgiSocket &cgiSocket);

    [[nodiscard]] HttpRequest &getHttpRequest() const;
    [[nodiscard]] HttpResponse &getHttpResponse() const;

  private:
    int statusCode_ = Http::StatusCode::OK;
    constexpr static size_t bufferSize_ = 4096;
    std::unique_ptr<HttpRequest> httpRequest_;
    std::unique_ptr<HttpResponse> httpResponse_;
    std::unique_ptr<Router> router_;
    std::unique_ptr<ClientSocket> client_socket_;
    Server &server_;
};