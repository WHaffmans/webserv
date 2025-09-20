#include "webserv/socket/Socket.hpp"
#include <iostream>
#include <webserv/client/Client.hpp>
#include <webserv/log/Log.hpp>

Client::Client(std::unique_ptr<Socket> socket, Server &server, const ServerConfig &server_config)
    : client_socket_(std::move(socket)), server(std::ref(server)), server_config(std::cref(server_config))
{
}

Client::~Client()
{
    LOG_INFO("Client destructor called for fd: " + std::to_string(client_socket_->getFd()));
    server.removeFromEpoll(*client_socket_);
};

int Client::parseHeaderforContentLength(const std::string &request) //NOLINT
{
    std::string header = "Content-Length: ";
    size_t pos = request.find(header);
    LOG_DEBUG("Parsing header for Content-Length...\n" + header);
    if (pos != std::string::npos)
    {
        size_t start = pos + header.length();
        size_t end = request.find("\r\n", start);
        if (end != std::string::npos)
        {
            std::string lengthStr = request.substr(start, end - start);
            return std::atoi(lengthStr.c_str());
        }
    }
    return -1; // Not found
}

void Client::request()
{
    char buffer[9] = {}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    ssize_t bytesRead =
        client_socket_->recv(buffer, sizeof(buffer) - 1); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    if (bytesRead < 0)
    {
        perror("Read error");
        return;
    }
    if (bytesRead == 0)
    {
        std::cout << "Client disconnected, fd: " << client_socket_->getFd() << '\n';
        return;
    }

    buffer[bytesRead] = '\0'; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    requestBuffer_ += buffer; // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    if (header_.empty())
    {
        auto headerEnd = requestBuffer_.find("\r\n\r\n");
        if (headerEnd != std::string::npos)
        {
            header_ = requestBuffer_.substr(0, headerEnd + 4);
            contentLength_ = parseHeaderforContentLength(header_);
            if (contentLength_ == -1)
            {
                LOG_INFO("Received complete request:\n" + requestBuffer_ + "\n=== HEADER FINISHED\n");
                server.responseReady(client_socket_->getFd());
            }
            requestBuffer_.erase(0, headerEnd + 4);
            return;
        }
    }
    else
    {
        content_ += requestBuffer_;
        if (content_.size() >= contentLength_)
        {
            LOG_INFO("Received complete request:\n" + header_ + content_ + "\n=== FULL REQUEST FINISHED\n");
            server.responseReady(client_socket_->getFd());
            requestBuffer_.clear();
            contentLength_ = -1;
        }
    }
}

std::string Client::getResponse() const
{
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 32\r\n\r\nHello, World!";
    response += " Server port " + std::to_string(server_config.getPort()) + "\r\n";
    LOG_DEBUG("Sending response:\n" + response);
    return response;
}