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
    : client_socket_(std::move(socket)), server_(std::ref(server)), httpRequest_(std::make_unique<HttpRequest>(this))
{
    Log::info("New client connected, fd: " + std::to_string(client_socket_->getFd()));
}

Client::~Client()
{
    Log::info("Client disconnected, fd: " + std::to_string(client_socket_->getFd()));
    server_.removeFromEpoll(*client_socket_);
};

int Client::getStatusCode() const
{
    return statusCode_;
}

void Client::setStatusCode(int code)
{
    statusCode_ = code;
}

void Client::request()
{
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
        Log::info("Client disconnected, fd: " + std::to_string(client_socket_->getFd())); //TODO weird
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
        server_config_ = ConfigManager::getInstance().getMatchingServerConfig(httpRequest_->getHeaders().get("Host"));
        if (server_config_ == nullptr)
        {
            Log::warning("No matching server config found for Host: " + httpRequest_->getHeaders().get("Host"));
            httpRequest_->setState(HttpRequest::State::ParseError);
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

std::string Client::getResponse() const
{
    Log::trace(LOCATION);
    if (httpRequest_->getState() == HttpRequest::State::ParseError)
    {
        return ErrorHandler::generateErrorPage(Http::StatusCode::BAD_REQUEST);
    }
    std::string response = "HTTP/1.1 ";
    response += "200 OK\r\n";

    auto serverName = server_config_->getDirectiveValue<std::string>("server_name");
    auto port = server_config_->getDirectiveValue<int>("listen");
    std::string body = "Server Name " + serverName + "\r\n";
    body += "Server port " + std::to_string(port) + "\r\n";
    response += "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
    response += body;
    Log::info("Prepared response for client fd: " + std::to_string(client_socket_->getFd()));
    Log::debug("Sending response:\n" + response);
    return response;
}