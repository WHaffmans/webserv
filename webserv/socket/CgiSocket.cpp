#include <webserv/socket/CgiSocket.hpp>

#include <webserv/log/Log.hpp>        // for LOCATION, Log
#include <webserv/socket/ASocket.hpp> // for ASocket

#include <system_error> // for generic_category, system_error

#include <errno.h>  // for errno
#include <unistd.h> // for read, write

CgiSocket::CgiSocket(int fd, ASocket::IoState event) : ASocket(fd, event)
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
    return bytesRead;
}

ssize_t CgiSocket::write(const void *buf, size_t len) const
{
    Log::trace(LOCATION);
    ssize_t bytesSent = ::write(getFd(), buf, len);
    return bytesSent;
}
