#include "webserv/socket/Socket.hpp"

#include <webserv/client/Client.hpp>
#include <webserv/log/Log.hpp>

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

    if (httpRequest_->getState() == HttpRequest::State::Complete)
    {
        Log::info("Received complete request:\n" + httpRequest_->getHeaders() + httpRequest_->getBody() +
                  "\n=== FULL REQUEST FINISHED\n");
        server_.responseReady(client_socket_->getFd());
        httpRequest_->reset();
    }
    else
    {
        Log::debug("Received partial request:\n" + httpRequest_->getHeaders() + httpRequest_->getBody() +
                   "\n=== PARTIAL REQUEST\n");
    }
}

std::string Client::getResponse() const
{
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 32\r\n\r\nHello, World!";
    response += " Server port " + std::to_string(server_config_.getPort()) + "\r\n";
    Log::debug("Sending response:\n" + response);
    return response;
}