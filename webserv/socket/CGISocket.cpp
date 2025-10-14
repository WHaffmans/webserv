#include <webserv/log/Log.hpp>
#include <webserv/socket/CGISocket.hpp>

CGISocket::CGISocket(int fd) : ASocket(fd)
{
    Log::trace(LOCATION);
}

ASocket::Type CGISocket::getType() const
{
    return ASocket::Type::CGI_SOCKET;
}
