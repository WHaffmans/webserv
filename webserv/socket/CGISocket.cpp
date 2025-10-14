#include <webserv/socket/CGISocket.hpp>

#include <webserv/log/Log.hpp>        // for LOCATION, Log
#include <webserv/socket/ASocket.hpp> // for ASocket

CGISocket::CGISocket(int fd) : ASocket(fd)
{
    Log::trace(LOCATION);
}

ASocket::Type CGISocket::getType() const
{
    return ASocket::Type::CGI_SOCKET;
}
