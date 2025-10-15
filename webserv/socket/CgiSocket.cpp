#include <webserv/log/Log.hpp>        // for LOCATION, Log
#include <webserv/socket/ASocket.hpp> // for ASocket
#include <webserv/socket/CgiSocket.hpp>

CgiSocket::CgiSocket(int fd) : ASocket(fd)
{
    Log::trace(LOCATION);
}

ASocket::Type CgiSocket::getType() const
{
    return ASocket::Type::CGI_SOCKET;
}
