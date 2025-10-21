#include <webserv/log/Log.hpp> // for Log, LOCATION
#include <webserv/socket/ASocket.hpp>

#include <stdexcept>    // for runtime_error
#include <string>       // for basic_string
#include <system_error> // for generic_category, system_error

#include <errno.h>      // for errno
#include <fcntl.h>      // for fcntl, F_SETFL, O_NONBLOCK
#include <sys/socket.h> // for recv, send
#include <unistd.h>     // for close

ASocket::ASocket(int fd, IOState event) : fd_(fd), ioState_(event)
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
    Log::trace(LOCATION + "Closing socket fd: " + std::to_string(fd_));
    if (fd_ != -1)
    {
        close(fd_);
    }
}

ssize_t ASocket::read(void *buf, size_t len) const
{
    Log::trace(LOCATION);
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

int ASocket::getFd() const noexcept
{
    return fd_;
}

ASocket::IOState ASocket::getEvent() const noexcept
{
    return ioState_;
}

bool ASocket::isDirty() const noexcept
{
    return dirty_;
}

void ASocket::setIOState(IOState event)
{
    if (event == ioState_)
    {
        return;
    }

    Log::debug("Processing state change for socket " + std::to_string(fd_));
    dirty_ = true;
    ioState_ = event;
}

void ASocket::processed()
{
    Log::debug("Socket " + std::to_string(fd_) + " processed");
    dirty_ = false;
}

void ASocket::setFd(int fd)
{
    fd_ = fd;
}

void ASocket::callback() const
{
    if (callback_ != nullptr)
    {
        callback_();
    }
}

void ASocket::setCallback(std::function<void()> callback)
{
    callback_ = std::move(callback);
}
