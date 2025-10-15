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
    void respond() const;
    void poll() const;

    [[nodiscard]] int getStatusCode() const;

    [[nodiscard]] ASocket &getSocket(int fd = -1) const;

    void setStatusCode(int code);
    void setCgiSocket(std::unique_ptr<CgiSocket> cgiSocket);

    [[nodiscard]] HttpRequest &getHttpRequest() const;
    [[nodiscard]] HttpResponse &getHttpResponse() const;

  private:
    int statusCode_ = Http::StatusCode::OK;
    constexpr static size_t bufferSize_ = 4096;
    std::unique_ptr<HttpRequest> httpRequest_;
    std::unique_ptr<HttpResponse> httpResponse_;
    std::unique_ptr<Router> router_;
    std::unique_ptr<ClientSocket> client_socket_;
    std::unique_ptr<CgiSocket> cgi_socket_ = nullptr;
    Server &server_;
};