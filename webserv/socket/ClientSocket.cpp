#include <webserv/log/Log.hpp>
#include <webserv/socket/ClientSocket.hpp>

ClientSocket::ClientSocket(int fd) : ASocket(fd)
{
    Log::trace(LOCATION);
}

ASocket::Type ClientSocket::getType() const
{
    Log::trace(LOCATION);
    return ASocket::Type::CLIENT_SOCKET;
}
