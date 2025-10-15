#include "webserv/socket/ASocket.hpp"
#include "webserv/socket/CgiSocket.hpp"

#include <webserv/client/Client.hpp>
#include <webserv/http/HttpHeaders.hpp>    // for HttpHeaders
#include <webserv/log/Log.hpp>             // for Log, LOCATION
#include <webserv/router/Router.hpp>       // for Router
#include <webserv/server/Server.hpp>       // for Server
#include <webserv/socket/ClientSocket.hpp> // for Socket

#include <cstdint> // for uint8_t
#include <exception>
#include <functional> // for ref, reference_wrapper
#include <map>        // for map
#include <memory>
#include <string>  // for basic_string, to_string, operator+, operator<=>
#include <utility> // for pair, move

#include <sys/epoll.h>
#include <sys/types.h> // for ssize_t

Client::Client(std::unique_ptr<ClientSocket> socket, Server &server)
    : httpRequest_(std::make_unique<HttpRequest>(this)), httpResponse_(std::make_unique<HttpResponse>()),
      router_(std::make_unique<Router>(this)), client_socket_(std::move(socket)), server_(std::ref(server))
{
    Log::trace(LOCATION);
    Log::info("New client connected, fd: " + std::to_string(client_socket_->getFd()));
    client_socket_->setCallback([this]() { request(); });
}

Client::~Client()
{
    Log::trace(LOCATION);
    Log::info("Client disconnected, fd: " + std::to_string(client_socket_->getFd()));
    server_.remove(*client_socket_);
};

int Client::getStatusCode() const
{
    return statusCode_;
}

void Client::setStatusCode(int code)
{
    statusCode_ = code;
}

ASocket &Client::getSocket(int fd) const
{
    if (fd == -1 || client_socket_->getFd() == fd)
    {
        return *client_socket_;
    }
    if (cgi_socket_ && cgi_socket_->getFd() == fd)
    {
        return *client_socket_; // TODO return cgi socket
    }
    Log::error("Socket not found for fd: " + std::to_string(fd));
    throw std::runtime_error("Socket not found for fd: " + std::to_string(fd));
}

void Client::request()
{
    Log::trace(LOCATION);
    char buffer[bufferSize_] = {}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    ssize_t bytesRead = client_socket_->read(
        buffer, sizeof(buffer) - 1); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    if (bytesRead < 0)
    {
        Log::error("Read error");
        throw std::runtime_error("Read error");
    }
    if (bytesRead == 0)
    {
        Log::info("Client closed connection, fd: " + std::to_string(client_socket_->getFd()));
        server_.disconnect(*this); // CRITICAL: RETURN IMMEDIATELY
        return;
    }

    buffer[bytesRead] = '\0'; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    httpRequest_->receiveData(static_cast<const char *>(buffer), static_cast<size_t>(bytesRead));

    if (httpRequest_->getState() == HttpRequest::State::Complete
        || httpRequest_->getState() == HttpRequest::State::ParseError)
    {
        Log::info("Received complete request",
                  {
                      {"request_method", httpRequest_->getMethod()},
                      {"request_target", httpRequest_->getTarget()},
                      {"http_version", httpRequest_->getHttpVersion()},
                      {"headers", httpRequest_->getHeaders().toString()},
                      {"body", httpRequest_->getBody()},
                      {"state", std::to_string(static_cast<uint8_t>(httpRequest_->getState()))},
                  });
        // server_.responseReady(client_socket_->getFd());
        router_->handleRequest();
    }
    else
    {
        Log::debug("Received partial request",
                   {
                       {"current_state", std::to_string(static_cast<uint8_t>(httpRequest_->getState()))},
                       {"buffer_length", std::to_string(bytesRead)},
                   });
    }
}

void Client::setCgiSocket(std::unique_ptr<CgiSocket> cgiSocket)
{
    server_.add(*cgiSocket, EPOLLIN, this);
    cgi_socket_ = std::move(cgiSocket);
    // TODO add to handler
}

void Client::poll() const
{
    if (httpResponse_->isComplete())
    {
        Log::info("Response is ready to be sent to client, fd: " + std::to_string(client_socket_->getFd()));
        client_socket_->setCallback([this]() { respond(); });
        server_.responseReady(client_socket_->getFd());
    }
}

void Client::respond() const
{
    auto payload = httpResponse_->toBytes();
    ssize_t bytesSent = send(client_socket_->getFd(), payload.data(), payload.size(), 0);
    if (bytesSent < 0)
    {
        Log::error("Send failed for fd: " + std::to_string(client_socket_->getFd()));
    }
    else
    {
        Log::debug("Sent " + std::to_string(bytesSent) + " bytes to fd: " + std::to_string(client_socket_->getFd()));
    }
}

HttpRequest &Client::getHttpRequest() const
{
    return *httpRequest_;
}

HttpResponse &Client::getHttpResponse() const
{
    return *httpResponse_;
}