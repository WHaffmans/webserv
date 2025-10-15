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
}

Client::~Client()
{
    Log::trace(LOCATION);
    Log::info("Client disconnected, fd: " + std::to_string(clientSocket_->getFd()));
    server_.remove(*clientSocket_);
};

int Client::getStatusCode() const noexcept
{
    return statusCode_;
}

void Client::setStatusCode(int code)
{
    statusCode_ = code;
}

ASocket &Client::getSocket(int fd) const
{
    if (fd == -1 || clientSocket_->getFd() == fd)
    {
        return *clientSocket_;
    }
    if (cgiStdIn_ && cgiStdIn_->getFd() == fd)
    {
        return *cgiStdIn_;
    }
    if (cgiStdOut_ && cgiStdOut_->getFd() == fd)
    {
        return *cgiStdOut_;
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

void Client::writeToCgi()
{
    Log::trace(LOCATION);
    if (cgiStdIn_ == nullptr)
    {
        Log::error("CGI stdin socket is null");
        return;
    }
    if (httpRequest_->getBody().empty())
    {
        Log::debug("No body to write to CGI stdin, fd: " + std::to_string(cgiStdIn_->getFd()));
        server_.remove(*cgiStdIn_);
        cgiStdIn_ = nullptr;
        return;
    }
    ssize_t bytesWritten = cgiStdIn_->write(httpRequest_->getBody().data(), httpRequest_->getBody().size());
    if (bytesWritten < 0)
    {
        Log::error("Failed to write to CGI stdin, fd: " + std::to_string(cgiStdIn_->getFd()));
    }
    else
    {
        Log::debug("Wrote " + std::to_string(bytesWritten)
                   + " bytes to CGI stdin, fd: " + std::to_string(cgiStdIn_->getFd()));
    }
    server_.remove(*cgiStdIn_);
    cgiStdIn_ = nullptr;
}

void Client::readFromCgi()
{
    Log::trace(LOCATION);
    if (cgiStdOut_ == nullptr)
    {
        Log::error("CGI stdout socket is null");
        return;
    }
    char buffer[bufferSize_] = {}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    ssize_t bytesRead
        = cgiStdOut_->read(buffer, sizeof(buffer) - 1); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    if (bytesRead < 0)
    {
        Log::error("Failed to read from CGI stdout, fd: " + std::to_string(cgiStdOut_->getFd()));
    }
    else if (bytesRead == 0)
    {
        Log::info("CGI process closed stdout, fd: " + std::to_string(cgiStdOut_->getFd()));
        server_.remove(*cgiStdOut_);
        cgiStdOut_ = nullptr;
        httpResponse_->addHeader("Content-Type", "text/html");
        httpResponse_->setComplete();
        return;
    }
    else
    {
        buffer[bytesRead] = '\0'; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        httpResponse_->appendBody(std::string(buffer, static_cast<size_t>(bytesRead)));
        Log::debug("Read " + std::to_string(bytesRead)
                   + " bytes from CGI stdout, fd: " + std::to_string(cgiStdOut_->getFd()));
    }
}

void Client::setCgiSockets(std::unique_ptr<CgiSocket> cgiStdIn, std::unique_ptr<CgiSocket> cgiStdOut)
{
    cgiStdIn->setCallback([this]() { writeToCgi(); });
    cgiStdOut->setCallback([this]() { readFromCgi(); });

    cgiStdOut_ = std::move(cgiStdOut);
    cgiStdIn_ = std::move(cgiStdIn);

    server_.add(*cgiStdOut_, EPOLLIN, this); // read
    server_.add(*cgiStdIn_, EPOLLOUT, this); // write

    // TODO add to handler
}

void Client::poll() const
{
    if (httpResponse_->isComplete())
    {
        Log::info("Response is ready to be sent to client, fd: " + std::to_string(clientSocket_->getFd()));
        clientSocket_->setCallback([this]() { respond(); });
        server_.responseReady(clientSocket_->getFd());
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