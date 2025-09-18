#include "webserv/socket/Socket.hpp"
#include <iostream>
#include <webserv/client/Client.hpp>

Client::Client(std::unique_ptr<Socket> socket, Server &server, const ServerConfig &server_config)
    : client_socket_(std::move(socket)), server(std::ref(server)), server_config(std::cref(server_config))
{
}

Client::~Client(){
    std::cout << "Client destructor called for fd: " << client_socket_->getFd() << '\n';
    server.removeFromEpoll(*client_socket_);
};

void Client::request()
{
    char buffer[1024] = {};
    ssize_t bytesRead = client_socket_->recv(buffer, sizeof(buffer) - 1);
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

    buffer[bytesRead] = '\0'; // Null-terminate the buffer
    std::cout << "Received request:\n" << buffer << '\n';

    // Handle the request (placeholder implementation)
    server.responseReady(client_socket_->getFd());
}


std::string Client::getResponse() const
{
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 32\r\n\r\nHello, World!";
    response += " Server port " + std::to_string(server_config.getPort()) + "\r\n";
    std::cout << response << '\n';
    return response;
}