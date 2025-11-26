#pragma once

// #include <webserv/http/HttpResponse.hpp>

#include <webserv/config/ServerConfig.hpp> // for ServerConfig
#include <webserv/handler/AHandler.hpp>    // for AHandler
#include <webserv/http/HttpConstants.hpp>  // for OK
#include <webserv/http/HttpRequest.hpp>    // for HttpRequest
#include <webserv/http/HttpResponse.hpp>   // for HttpResponse
#include <webserv/router/Router.hpp>
#include <webserv/server/Server.hpp>
#include <webserv/socket/ASocket.hpp>
#include <webserv/socket/CgiSocket.hpp>
#include <webserv/socket/ClientSocket.hpp> // for ClientSocket

#include <cstddef>       // for size_t
#include <memory>        // for unique_ptr
#include <string>        // for string
#include <unordered_map> // for unordered_map

class Server;
class ClientSocket;
class ServerConfig;
class HttpResponse;
class ASocket;
class HttpRequest;
class Router;

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
    void respond();
    void poll();

    [[nodiscard]] ASocket &getSocket(int fd = -1) const;

    void addSocket(ASocket *socket);
    void removeSocket(ASocket *socket);

  

    [[nodiscard]] ClientSocket *getClientSocket() const noexcept;
    [[nodiscard]] HttpRequest &getHttpRequest() const noexcept;
    [[nodiscard]] HttpResponse &getHttpResponse() const noexcept;
    [[nodiscard]] std::string getClientAddress() const noexcept;

  private:
    // int statusCode_ = Http::StatusCode::OK;
    std::unique_ptr<HttpRequest> httpRequest_;
    std::unique_ptr<HttpResponse> httpResponse_;
    std::unique_ptr<Router> router_;
    std::unique_ptr<ClientSocket> clientSocket_;
    std::unique_ptr<TimerSocket> timerSocket_;
    std::unique_ptr<AHandler> handler_ = nullptr;
    std::unordered_map<int, ASocket *> sockets_;

    Server &server_;
    long writeOffset_ = 0;
    void startTimer();
    void resetTimer();
    void handleTimeout();
    // void writeToCgi();
    // void readFromCgi();
};