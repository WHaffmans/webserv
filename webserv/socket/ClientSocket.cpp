#include <webserv/socket/ClientSocket.hpp>

#include <webserv/log/Log.hpp>        // for LOCATION, Log
#include <webserv/socket/ASocket.hpp> // for ASocket

ClientSocket::ClientSocket(int fd) : ASocket(fd)
{
    Log::trace(LOCATION);
}

ASocket::Type ClientSocket::getType() const
{
    Log::trace(LOCATION);
    return ASocket::Type::CLIENT_SOCKET;
}
