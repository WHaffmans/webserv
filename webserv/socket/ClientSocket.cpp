#include <webserv/socket/ClientSocket.hpp>

#include <webserv/log/Log.hpp>        // for LOCATION, Log
#include <webserv/socket/ASocket.hpp> // for ASocket

ClientSocket::ClientSocket(int fd, struct sockaddr address) : ASocket(fd), address_(address)
{
    Log::trace(LOCATION);
}

ASocket::Type ClientSocket::getType() const noexcept
{
    return ASocket::Type::CLIENT_SOCKET;
}

const struct sockaddr *ClientSocket::getAddress() const noexcept
{
    return &address_;
}