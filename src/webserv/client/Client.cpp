#include "webserv/http/HttpConstants.hpp"

#include <webserv/client/Client.hpp>
#include <webserv/handler/CgiHandler.hpp>    // for CgiHandler
#include <webserv/handler/ErrorHandler.hpp>  // for ErrorHandler
#include <webserv/handler/URI.hpp>           // for URI
#include <webserv/http/HttpRequest.hpp>      // for HttpRequest
#include <webserv/http/HttpResponse.hpp>     // for HttpResponse
#include <webserv/http/RequestValidator.hpp> // for RequestValidator
#include <webserv/log/Log.hpp>               // for Log, LOCATION
#include <webserv/main.hpp>                  // for BUFFER_SIZE
#include <webserv/router/Router.hpp>         // for Router
#include <webserv/server/Server.hpp>         // for Server
#include <webserv/socket/ASocket.hpp>        // for ASocket
#include <webserv/socket/ClientSocket.hpp>   // for ClientSocket

#include <exception>  // for exception
#include <functional> // for function, ref, reference_wrapper
#include <memory>     // for unique_ptr, allocator, make_unique, operator==, default_delete
#include <stdexcept>  // for runtime_error
#include <string>     // for operator+, basic_string, to_string, char_traits, string
#include <utility>    // for move, pair
#include <vector>     // for vector

#include <arpa/inet.h>  // for inet_ntoa
#include <netinet/in.h> // for in_addr, sockaddr_in
#include <stddef.h>     // for size_t
#include <sys/socket.h> // for send, AF_INET, sockaddr
#include <sys/types.h>  // for ssize_t

Client::Client(std::unique_ptr<ClientSocket> socket, Server &server)
    : httpRequest_(std::make_unique<HttpRequest>(this)), httpResponse_(std::make_unique<HttpResponse>()),
      router_(std::make_unique<Router>(this)), clientSocket_(std::move(socket)), server_(std::ref(server))
{
    Log::trace(LOCATION);
    Log::info(clientSocket_->toString() + ": connected");
    clientSocket_->setCallback([this]() { request(); });
    sockets_[clientSocket_->getFd()] = clientSocket_.get();
}

Client::~Client()
{
    Log::trace(LOCATION);
    Log::info(clientSocket_->toString() + ": disconnected");
    for (auto it : sockets_)
    {
        server_.remove(*(it.second));
    }
    // server_.remove(*clientSocket_);
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

void Client::cleanHandler(AHandler *handler)
{
    if (handler == nullptr)
    {
        return;
    }
    Log::debug(clientSocket_->toString() + ": cleaning up handler");
    // Extract timer socket if it exists and remove from tracking
    auto *timerSocket = handler->getTimerSocket();
    if (timerSocket != nullptr)
    {
        removeSocket(timerSocket);
    }
    // Extract CGI sockets if this was a CgiHandler
    auto *cgiHandler = dynamic_cast<CgiHandler *>(handler);
    if (cgiHandler != nullptr)
    {
        auto *cgiProcess = cgiHandler->getCgiProcess();
        if (cgiProcess != nullptr)
        {
            if (auto *stdinSocket = cgiHandler->getCgiStdIn())
            {
                removeSocket(stdinSocket);
            }
            if (auto *stdoutSocket = cgiHandler->getCgiStdOut())
            {
                removeSocket(stdoutSocket);
            }
            if (auto *stderrSocket = cgiHandler->getCgiStdErr())
            {
                removeSocket(stderrSocket);
            }
        }
    }
}

void Client::request()
{
    Log::trace(LOCATION);
    char buffer[Constants::BUFFER_SIZE] = {}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    ssize_t bytesRead = clientSocket_->read(
        buffer, sizeof(buffer) - 1); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    if (bytesRead < 0)
    {
        Log::error("Read error");
        throw std::runtime_error("Read error");
    }
    if (bytesRead == 0)
    {
        Log::info(clientSocket_->toString() + ": closed connection");
        server_.disconnect(*this); // CRITICAL: RETURN IMMEDIATELY
        return;
    }

    buffer[bytesRead] = '\0'; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    if (httpRequest_->getState() == HttpRequest::State::Complete)
    {
        Log::warning(clientSocket_->toString() + ": received data after request was already complete; ignoring");
        return;
    }
    httpRequest_->receiveData(static_cast<const char *>(buffer), static_cast<size_t>(bytesRead));

    // If parsing failed, proactively send an error response (avoid timeouts on malformed requests)
    if (httpRequest_->getState() == HttpRequest::State::ParseError)
    {
        Log::warning(clientSocket_->toString() + ": request parsing failed; preparing error response");
        if (!httpResponse_->isComplete())
        {
            ErrorHandler::createErrorResponse(400, *httpResponse_);
        }
        clientSocket_->setCallback([this]() { respond(); });
        clientSocket_->setIOState(ASocket::IoState::WRITE);
        return;
    }

    if (httpRequest_->getState() == HttpRequest::State::Complete)
    {
        Log::info(clientSocket_->toString() + ": " + httpRequest_->getMethod() + " " + httpRequest_->getTarget());
        try
        {
            // if (handler_ != nullptr)
            // {
            //     cleanHandler(handler_.get());
            // }
            handler_ = router_->handleRequest();
            if (handler_ != nullptr)
            {
                handler_->handle();
            }
        }
        catch (const RequestValidator::ValidationException &e)
        {
            Log::debug(clientSocket_->toString()
                       + ": validation exception during request handling: " + std::string(e.what()));
            const auto &config = httpRequest_->getUri().getConfig();
            ErrorHandler::createErrorResponse(e.code(), *httpResponse_, config);
            return;
        }
        catch (const std::exception &e)
        {
            Log::error(clientSocket_->toString() + ": exception during request handling: " + std::string(e.what()));
            ErrorHandler::createErrorResponse(500, *httpResponse_);
            return;
        }
    }
    else
    {
        Log::debug("Received partial request");
    }
}

void Client::addSocket(ASocket *socket)
{
    server_.add(*socket, this);
    sockets_[socket->getFd()] = socket;
}

void Client::removeSocket(ASocket *socket)
{
    server_.remove(*socket);
    sockets_.erase(socket->getFd());
}

void Client::poll()
{
    auto *cgiHandler = dynamic_cast<CgiHandler *>(handler_.get());
    if (cgiHandler != nullptr)
    {
        cgiHandler->wait();
    }
    if (httpResponse_->isComplete() && clientSocket_->getEvent() != ASocket::IoState::WRITE)
    {
        auto statusCode = httpResponse_->getStatusCode();
        Log::info(clientSocket_->toString() + ": " + std::to_string(statusCode) + " "
                  + Http::getStatusCodeReason(statusCode));
        clientSocket_->setCallback([this]() { respond(); });
        clientSocket_->setIOState(ASocket::IoState::WRITE);
    }
}

void Client::respond()
{
    auto payload = httpResponse_->toBytes(writeOffset_);
    ssize_t bytesSent = send(clientSocket_->getFd(), payload.data(), payload.size(), 0);
    if (bytesSent < 0)
    {
        Log::error(clientSocket_->toString() + ": send failed");
    }
    else
    {
        Log::debug(clientSocket_->toString() + ": sent " + std::to_string(bytesSent) + " bytes out of "
                   + std::to_string(writeOffset_ + payload.size()) + "(offset: " + std::to_string(writeOffset_) + ")");
        writeOffset_ += bytesSent;
    }
    if (payload.empty())
    {
        Log::debug(clientSocket_->toString() + ": closing connection to client");
        server_.disconnect(*this); // ! CRITICAL: RETURN IMMEDIATELY
    }
}

HttpRequest &Client::getHttpRequest() const noexcept
{
    return *httpRequest_;
}

HttpResponse &Client::getHttpResponse() const noexcept
{
    return *httpResponse_;
}

// NOLINTBEGIN
std::string Client::getClientAddress() const noexcept
{
    const struct sockaddr *addr = clientSocket_->getAddress();
    if (addr->sa_family == AF_INET)
    {
        const struct sockaddr_in *ipv4 = reinterpret_cast<const struct sockaddr_in *>(addr);
        const char *addr = inet_ntoa(ipv4->sin_addr);
        return std::string(addr);
    }

    return "";
}

ClientSocket *Client::getClientSocket() const noexcept
{
    return clientSocket_.get();
}

// NOLINTEND