#include <webserv/socket/ServerSocket.hpp>

#include <webserv/log/Log.hpp>             // for Log, LOCATION
#include <webserv/socket/ASocket.hpp>      // for ASocket
#include <webserv/socket/ClientSocket.hpp> // for ClientSocket

#include <memory>    // for allocator, make_unique, unique_ptr
#include <stdexcept> // for runtime_error

#include <arpa/inet.h>  // for htons, inet_addr
#include <netinet/in.h> // for sockaddr_in, in_addr
#include <stdint.h>     // for uint16_t
#include <sys/socket.h> // for AF_INET, accept, bind, listen, setsockopt, socket, SOCK_STREAM, SOL_SOCKET, SO_REUSEADDR
#include <unistd.h>     // for close

ServerSocket::ServerSocket(const std::string &host, int port)
    : ASocket(socket(AF_INET, SOCK_STREAM, 0), ASocket::IoState::READ), host_(host), port_(port)
{
    Log::trace(LOCATION);
    if (getFd() == -1)
    {
        Log::error("Socket creation failed");
        throw std::runtime_error("Socket creation failed");
    }
    int opt = 1;
    if (setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(getFd());
        setFd(-1);
        Log::error("setsockopt failed");
        throw std::runtime_error("setsockopt failed");
    }
    bind(host_, port_);
    listen(SOMAXCONN);
}

void ServerSocket::listen(int backlog) const
{
    Log::trace(LOCATION);
    if (::listen(getFd(), backlog) < 0)
    {
        Log::error("Listen failed");
        throw std::runtime_error("Listen failed");
    }
}

void ServerSocket::bind(const std::string &host, const int port) const
{
    Log::trace(LOCATION);
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host.c_str());
    address.sin_port = htons(static_cast<uint16_t>(port));

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    if (::bind(getFd(), reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0)
    {
        Log::fatal("Cannot bind to " + host + ":" + std::to_string(port)
                   + " - address already in use or permission denied");
        throw std::runtime_error("Bind failed");
    }
}

ASocket::Type ServerSocket::getType() const noexcept
{
    return ASocket::Type::SERVER_SOCKET;
}

std::unique_ptr<ClientSocket> ServerSocket::accept() const
{
    Log::trace(LOCATION);
    struct sockaddr client_address{};
    socklen_t address_len = sizeof(client_address);
    int client_fd = ::accept(getFd(), &client_address, &address_len);
    if (client_fd < 0)
    {
        Log::error("Accept failed");
        throw std::runtime_error("Accept failed");
    }
    return std::make_unique<ClientSocket>(client_fd, client_address);
}

std::string ServerSocket::toString() const
{
    return "ServerSocket(fd=" + std::to_string(getFd()) + ")";
}

const std::string &ServerSocket::getHost() const noexcept
{
    return host_;
}

int ServerSocket::getPort() const noexcept
{
    return port_;
}