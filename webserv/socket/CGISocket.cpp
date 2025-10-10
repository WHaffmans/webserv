#include <webserv/socket/CGISocket.hpp>

#include <cstring>      // for strerror
#include <stdexcept>    // for runtime_error
#include <string>       // for string, operator+
#include <system_error> // for system_error, error_code

#include <fcntl.h>  // for fcntl, O_NONBLOCK
#include <unistd.h> // for close, read, write

CGISocket::CGISocket(int readFd, int writeFd) : _readFd(readFd), _writeFd(writeFd)
{
    if (_readFd == -1 || _writeFd == -1)
    {
        throw std::runtime_error("CGISocket: Invalid file descriptors");
    }
    setNonBlocking();
}

CGISocket::~CGISocket()
{
    if (_readFd != -1)
    {
        close(_readFd);
    }
    if (_writeFd != -1)
    {
        close(_writeFd);
    }

}

int CGISocket::getReadFd() const
{
    return _readFd;
}

int CGISocket::getWriteFd() const
{
    return _writeFd;
}

ssize_t CGISocket::read(void *buffer, size_t size) const
{
    ssize_t bytesRead = ::read(_readFd, buffer, size);
    if (bytesRead == -1)
    {
        throw std::system_error(errno, std::generic_category(), "CGISocket: Read error");
    }
    return bytesRead;
}

ssize_t CGISocket::write(const void *buffer, size_t size) const
{
    ssize_t bytesWritten = ::write(_writeFd, buffer, size);
    if (bytesWritten == -1)
    {
        throw std::system_error(errno, std::generic_category(), "CGISocket: Write error");
    }
    return bytesWritten;
}

void CGISocket::setNonBlocking() const
{
    if (fcntl(_readFd, F_SETFL, O_NONBLOCK) == -1)
    {
        throw std::system_error(errno, std::generic_category(), "CGISocket: Failed to set read FD non-blocking");
    }
    if (fcntl(_writeFd, F_SETFL, O_NONBLOCK) == -1)
    {
        throw std::system_error(errno, std::generic_category(), "CGISocket: Failed to set write FD non-blocking");
    }
}

