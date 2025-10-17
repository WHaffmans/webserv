#include "webserv/handler/CgiHandler.hpp"
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
      router_(std::make_unique<Router>(this)), clientSocket_(std::move(socket)), server_(std::ref(server))
{
    Log::trace(LOCATION);
    Log::info("New client connected, fd: " + std::to_string(clientSocket_->getFd()));
    clientSocket_->setCallback([this]() { request(); });
    sockets_[clientSocket_->getFd()] = clientSocket_.get();
}

Client::~Client()
{
    Log::trace(LOCATION);
    Log::info("Client disconnected, fd: " + std::to_string(clientSocket_->getFd()));
    server_.remove(*clientSocket_);
};

ASocket &Client::getSocket(int fd) const
{
    if (fd == -1)
    {
        return *clientSocket_;
    }
    if (sockets_.contains(fd))
    {
        return *sockets_.at(fd);
    }
    Log::error("Socket not found for fd: " + std::to_string(fd));
    throw std::runtime_error("Socket not found for fd: " + std::to_string(fd));
}

void Client::request()
{
    Log::trace(LOCATION);
    char buffer[bufferSize_] = {}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    ssize_t bytesRead = clientSocket_->read(
        buffer, sizeof(buffer) - 1); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    if (bytesRead < 0)
    {
        Log::error("Read error");
        throw std::runtime_error("Read error");
    }
    if (bytesRead == 0)
    {
        Log::info("Client closed connection, fd: " + std::to_string(clientSocket_->getFd()));
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
        handler_ = router_->handleRequest();
        handler_->handle();
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

//
void Client::setCgiSockets(CgiSocket *cgiStdIn, CgiSocket *cgiStdOut)
{
    server_.add(*cgiStdIn, EPOLLOUT, this); // write
    server_.add(*cgiStdOut, EPOLLIN, this); // read

    sockets_[cgiStdIn->getFd()] = cgiStdIn;
    sockets_[cgiStdOut->getFd()] = cgiStdOut;
}

void Client::removeCgiSocket(CgiSocket *cgiSocket)
{
    server_.remove(*cgiSocket);  // write

    sockets_.erase(cgiSocket->getFd());
    // sockets_[cgiStdIn->getFd()] = cgiStdIn;
    // sockets_[cgiStdOut->getFd()] = cgiStdOut;
}

void Client::poll() const
{
    auto * cgiHandler = dynamic_cast<CgiHandler *>(handler_.get());
    if (cgiHandler != nullptr)
    {
        Log::debug("Polling CGI handler for client, fd: " + std::to_string(clientSocket_->getFd()));
        // CGI handler polling logic if needed
        cgiHandler->wait();
    }
    if (httpResponse_->isComplete())
    {
        Log::info("Response is ready to be sent to client, fd: " + std::to_string(clientSocket_->getFd()));
        clientSocket_->setCallback([this]() { respond(); });
        server_.writable(clientSocket_->getFd());
    }

}

void Client::respond() const
{
    auto payload = httpResponse_->toBytes();
    ssize_t bytesSent = send(clientSocket_->getFd(), payload.data(), payload.size(), 0);
    if (bytesSent < 0)
    {
        Log::error("Send failed for fd: " + std::to_string(clientSocket_->getFd()));
    }
    else
    {
        Log::debug("Sent " + std::to_string(bytesSent) + " bytes to fd: " + std::to_string(clientSocket_->getFd()));
    }
    server_.disconnect(*this); // ! CRITICAL: RETURN IMMEDIATELY
}

HttpRequest &Client::getHttpRequest() const noexcept
{
    return *httpRequest_;
}

HttpResponse &Client::getHttpResponse() const noexcept
{
    return *httpResponse_;
}