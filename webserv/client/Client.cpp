#include "webserv/http/HttpConstants.hpp"
#include <webserv/client/Client.hpp>
#include <webserv/config/ConfigManager.hpp> // for ConfigManager
#include <webserv/config/ServerConfig.hpp>  // for ServerConfig
#include <webserv/handler/ErrorHandler.hpp> // for ErrorHandler
#include <webserv/http/HttpHeaders.hpp>     // for HttpHeaders
#include <webserv/log/Log.hpp>              // for Log, LOCATION
#include <webserv/server/Server.hpp>        // for Server
#include <webserv/socket/Socket.hpp>        // for Socket

#include <cstdint>    // for uint8_t
#include <functional> // for ref, reference_wrapper
#include <map>        // for map
#include <utility>    // for pair, move

#include <sys/types.h> // for ssize_t

Client::Client(std::unique_ptr<Socket> socket, Server &server)
    : client_socket_(std::move(socket)), server_(std::ref(server)), httpRequest_(std::make_unique<HttpRequest>(this)),
      httpResponse_(std::make_unique<HttpResponse>())
{
    Log::trace(LOCATION);
    Log::info("New client connected, fd: " + std::to_string(client_socket_->getFd()));
}

Client::~Client()
{
    Log::trace(LOCATION);
    Log::info("Client disconnected, fd: " + std::to_string(client_socket_->getFd()));
    server_.removeFromEpoll(*client_socket_);
};

int Client::getStatusCode() const
{
    Log::trace(LOCATION);
    return statusCode_;
}

void Client::setStatusCode(int code)
{
    Log::trace(LOCATION);
    statusCode_ = code;
}

void Client::request()
{
    Log::trace(LOCATION);
    char buffer[bufferSize_] = {}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    ssize_t bytesRead =
        client_socket_->recv(buffer, sizeof(buffer) - 1); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    if (bytesRead < 0)
    {
        Log::error("Read error");
        return;
    }
    if (bytesRead == 0)
    {
        Log::info("Client closed connection, fd: " + std::to_string(client_socket_->getFd())); // TODO weird
        server_.removeClient(*this); // CRITICAL: RETURN IMMEDIATELY
        return;
    }

    buffer[bytesRead] = '\0'; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    httpRequest_->receiveData(buffer, static_cast<size_t>(bytesRead));

    if (httpRequest_->getState() == HttpRequest::State::Complete ||
        httpRequest_->getState() == HttpRequest::State::ParseError)
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
        server_config_ =
            ConfigManager::getInstance().getMatchingServerConfig(httpRequest_->getHeaders().getHost().value_or(""));
        if (server_config_ == nullptr)
        {
            Log::warning("No matching server config found for Host: " +
                         httpRequest_->getHeaders().getHost().value_or("unknown host"));
            setError(Http::StatusCode::BAD_REQUEST);
        }

        // Example usage, replace with actual host and port extraction from request
        server_.responseReady(client_socket_->getFd());
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

bool Client::isResponseReady() const
{
    Log::trace(LOCATION);
    // todo: poll the httpResponse_ object
    return httpResponse_->isComplete();
}

std::vector<uint8_t> Client::getResponse() const
{
    Log::trace(LOCATION);
    Log::trace(LOCATION);
    if (statusCode_ == Http::StatusCode::OK)
    {
        httpResponse_->setStatus(200);
        httpResponse_->addHeader("Content-Type", "text/plain");
        httpResponse_->appendBody("Hello, World!\n");
    }
    return httpResponse_->toBytes();
}

void Client::setError(int statusCode)
{
    Log::trace(LOCATION);
    statusCode_ = statusCode;
    Log::debug("Setting error response with status code: " + std::to_string(statusCode));
    auto errorResponse = std::make_unique<HttpResponse>(
        ErrorHandler::getErrorResponse(statusCode, const_cast<ServerConfig *>(server_config_)));
    httpResponse_ = std::move(errorResponse);
    Log::debug("Error response set successfully");
}
