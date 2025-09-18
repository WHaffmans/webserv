#include <iostream>
#include <webserv/client/Client.hpp>

Client::Client(int client_fd, Server &server, const ServerConfig &server_config)
    : client_fd(client_fd), server(std::ref(server)), server_config(std::cref(server_config))
{
}

void Client::request(const std::string &req)
{
    // Handle the request (placeholder implementation)
    std::cout << "Client received request: " << req << '\n';
    server.get().responseReady(client_fd);
}

std::string Client::getResponse() const
{
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 32\r\n\r\nHello, World!";
    response += " Server port " + std::to_string(server_config.get().getPort()) + "\r\n";
    std::cout << response << '\n';
    return response;
}