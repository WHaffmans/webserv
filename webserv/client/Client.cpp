#include <webserv/client/Client.hpp>
#include <webserv/handler/CgiHandler.hpp>   // for CgiHandler
#include <webserv/handler/ErrorHandler.hpp> // for ErrorHandler
#include <webserv/http/HttpHeaders.hpp>     // for HttpHeaders
#include <webserv/http/HttpRequest.hpp>     // for HttpRequest
#include <webserv/http/HttpResponse.hpp>    // for HttpResponse
#include <webserv/http/RequestValidator.hpp>
#include <webserv/log/Log.hpp>             // for Log, LOCATION
#include <webserv/router/Router.hpp>       // for Router
#include <webserv/server/Server.hpp>       // for Server
#include <webserv/socket/ASocket.hpp>      // for ASocket
#include <webserv/socket/ClientSocket.hpp> // for ClientSocket

#include <cstdint>    // for uint8_t
#include <functional> // for function, ref, reference_wrapper
#include <map>        // for map
#include <memory>     // for unique_ptr, make_unique, allocator, operator==, default_delete
#include <stdexcept>  // for runtime_error
#include <string>     // for basic_string, to_string, operator+, char_traits, operator<=>
#include <utility>    // for move, pair
#include <vector>     // for vector

#include <sys/socket.h> // for send
#include <sys/types.h>  // for ssize_t

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
        Log::info("Received request: " + httpRequest_->getHttpVersion() + " " + httpRequest_->getMethod() + " "
                  + httpRequest_->getTarget() + " ");
                  
                  Log::debug("Request details",
                             {
                                 {"request_method", httpRequest_->getMethod()},
                                 {"request_target", httpRequest_->getTarget()},
                                 {"http_version", httpRequest_->getHttpVersion()},
                                 {"headers", httpRequest_->getHeaders().toString()},
                                 {"body", httpRequest_->getBody()},
                                 {"state", std::to_string(static_cast<uint8_t>(httpRequest_->getState()))},
                             });

                  try
                  {
                      // Thoughts: if a handler isn't returned, this could because of the error handler already setting
                      // up the response so, maybe we don't need to throw a 500 when no handler. Because that would
                      // override the actual error response. How about the router, or a handler, throws an exception if
                      // something goes wrong, and we catch it here to make a 500 response?
                      handler_ = router_->handleRequest();
                      if (handler_ != nullptr)
                      {
                          handler_->handle();
                      }
                  }
                  catch (const RequestValidator::ValidationException &e)
                  {
                      Log::error("Validation Exception during request handling: " + std::string(e.what()));
                      ErrorHandler::createErrorResponse(e.code(), *httpResponse_);
                      return;
                  }
                  catch (const std::exception &e)
                  {
                      Log::error("Exception during request handling: " + std::string(e.what()));
                      ErrorHandler::createErrorResponse(500, *httpResponse_);
                      return;
                  }
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

void Client::poll() const
{
    auto *cgiHandler = dynamic_cast<CgiHandler *>(handler_.get());
    if (cgiHandler != nullptr)
    {
        // Log::debug("Polling CGI handler for client, fd: " + std::to_string(clientSocket_->getFd()));
        // CGI handler polling logic if needed
        cgiHandler->wait();
    }
    if (httpResponse_->isComplete() && clientSocket_->getEvent() != ASocket::IoState::WRITE)
    {
        Log::info("Response is ready to be sent to client, fd: " + std::to_string(clientSocket_->getFd()));
        clientSocket_->setCallback([this]() { respond(); });
        // server_.writable(clientSocket_->getFd());
        clientSocket_->setIOState(ASocket::IoState::WRITE);
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