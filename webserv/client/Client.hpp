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
#include <memory>  // for unique_ptr

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

    void writeToCgi();
    void readFromCgi();


    // [[nodiscard]] int getStatusCode() const noexcept;

    [[nodiscard]] ASocket &getSocket(int fd = -1) const;

    // void setStatusCode(int code);
    void setCgiSockets(std::unique_ptr<CgiSocket> cgiStdIn, std::unique_ptr<CgiSocket> cgiStdOut);

    [[nodiscard]] HttpRequest &getHttpRequest() const noexcept;
    [[nodiscard]] HttpResponse &getHttpResponse() const noexcept;

  private:
    // int statusCode_ = Http::StatusCode::OK;
    constexpr static size_t bufferSize_ = 8192; // 8KB buffer for reading CGI output
    std::unique_ptr<HttpRequest> httpRequest_;
    std::unique_ptr<HttpResponse> httpResponse_;
    std::unique_ptr<Router> router_;
    std::unique_ptr<ClientSocket> clientSocket_;
    std::unique_ptr<CgiSocket> cgiStdIn_ = nullptr;
    std::unique_ptr<CgiSocket> cgiStdOut_ = nullptr;
    Server &server_;
};