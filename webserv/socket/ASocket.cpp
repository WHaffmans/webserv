#include <webserv/socket/ASocket.hpp>

#include <webserv/log/Log.hpp> // for Log, LOCATION

#include <stdexcept>    // for runtime_error
#include <string>       // for basic_string
#include <system_error> // for generic_category, system_error

#include <errno.h>      // for errno
#include <fcntl.h>      // for fcntl, F_SETFL, O_NONBLOCK
#include <sys/socket.h> // for recv, send
#include <unistd.h>     // for close

ASocket::ASocket(int fd) : fd_(fd)
{
    Log::trace(LOCATION);
    if (fd_ == -1)
    {
        Log::error("Invalid file descriptor");
        throw std::runtime_error("Invalid file descriptor");
    }
    setNonBlocking();
}

ASocket::~ASocket()
{
    Log::trace(LOCATION);
    if (fd_ != -1)
    {
        close(fd_);
    }
}

ssize_t ASocket::read(void *buf, size_t len) const
{
    ssize_t bytesRead = ::recv(fd_, buf, len, 0);
    if (bytesRead == -1)
    {
        throw std::system_error(errno, std::generic_category(), "Socket: Read error");
    }
    return bytesRead;
}

ssize_t ASocket::write(const void *buf, size_t len) const
{
    Log::trace(LOCATION);
    ssize_t bytesSent = ::send(fd_, buf, len, 0);
    if (bytesSent == -1)
    {
        throw std::system_error(errno, std::generic_category(), "Socket: Write error");
    }
    return bytesSent;
}

void ASocket::setNonBlocking() const
{
    if (fcntl(fd_, F_SETFL, O_NONBLOCK) == -1)
    {
        throw std::system_error(errno, std::generic_category(), "ASocket: Failed to set FD non-blocking");
    }
}

int ASocket::getFd() const
{
    Log::trace(LOCATION);
    return fd_;
}

void ASocket::setFd(int fd)
{
    fd_ = fd;
}