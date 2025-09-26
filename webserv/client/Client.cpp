#include <webserv/client/Client.hpp>
#include <webserv/http/HttpHeaders.hpp> // for HttpHeaders
#include <webserv/log/Log.hpp>          // for Log, LOCATION
#include <webserv/server/Server.hpp>    // for Server
#include <webserv/socket/Socket.hpp>    // for Socket

#include <cstdint>    // for uint8_t
#include <functional> // for reference_wrapper, cref, ref
#include <map>        // for map
#include <utility>    // for pair, move

#include <sys/types.h> // for ssize_t

class ServerConfig;

Client::Client(std::unique_ptr<Socket> socket, Server &server, const ServerConfig &server_config)
    : client_socket_(std::move(socket)), server_(std::ref(server)), server_config_(std::cref(server_config)),
      httpRequest_(std::make_unique<HttpRequest>(&server_config, this))
{
}

Client::~Client()
{
    Log::debug("Client destructor called for fd: " + std::to_string(client_socket_->getFd()));
    server_.removeFromEpoll(*client_socket_);
};

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
        Log::info("Client disconnected, fd: " + std::to_string(client_socket_->getFd()));
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
    std::string response = "HTTP/1.1 ";
    if (httpRequest_->getState() == HttpRequest::State::ParseError)
    {
        response += "400 Bad Request\r\n";
    }
    else
    {
        response += "200 OK\r\n";
    }
    // further validation can be added here
    response += "Content-Length: 18\r\n\r\n";
    response += "Server port 8080\r\n";

    Log::debug("Sending response:\n" + response);
    return response;
}