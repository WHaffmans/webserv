#include <webserv/log/Log.hpp>        // for LOCATION, Log
#include <webserv/socket/ASocket.hpp> // for ASocket
#include <webserv/socket/CgiSocket.hpp>

#include <unistd.h>

CgiSocket::CgiSocket(int fd, ASocket::IOState event) : ASocket(fd, event)
{
    Log::trace(LOCATION);
}

ASocket::Type CgiSocket::getType() const noexcept
{
    return ASocket::Type::CGI_SOCKET;
}

ssize_t CgiSocket::read(void *buf, size_t len) const
{
    Log::trace(LOCATION);
    ssize_t bytesRead = ::read(getFd(), buf, len);
    if (bytesRead == -1)
    {
        throw std::system_error(errno, std::generic_category(), "Socket: Read error");
    }
    return bytesRead;
}

ssize_t CgiSocket::write(const void *buf, size_t len) const
{
    Log::trace(LOCATION);
    ssize_t bytesSent = ::write(getFd(), buf, len);
    if (bytesSent == -1)
    {
        throw std::system_error(errno, std::generic_category(), "Socket: Write error");
    }
    return bytesSent;
}
