#include <webserv/log/Log.hpp>        // for LOCATION, Log
#include <webserv/socket/ASocket.hpp> // for ASocket
#include <webserv/socket/ClientSocket.hpp>

ClientSocket::ClientSocket(int fd) : ASocket(fd)
{
    Log::trace(LOCATION);
}

ASocket::Type ClientSocket::getType() const noexcept
{
    return ASocket::Type::CLIENT_SOCKET;
}
