#include <webserv/socket/ASocket.hpp>

#include <webserv/log/Log.hpp> // for Log, LOCATION

#include <stdexcept>    // for runtime_error
#include <string>       // for operator+, basic_string, to_string, allocator, char_traits, string
#include <system_error> // for generic_category, system_error
#include <utility>      // for move

#include <errno.h>      // for errno
#include <fcntl.h>      // for fcntl, FD_CLOEXEC, F_GETFD, F_GETFL, O_NONBLOCK, F_SETFD, F_SETFL
#include <sys/socket.h> // for recv, send
#include <unistd.h>     // for close

ASocket::ASocket(int fd, IoState event) : fd_(fd), ioState_(event)
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

    return bytesRead;
}

ssize_t ASocket::write(const void *buf, size_t len) const
{
    Log::trace(LOCATION);
    ssize_t bytesSent = ::send(fd_, buf, len, 0);

    return bytesSent;
}

void ASocket::setNonBlocking() const
{
    // Set file status flags (O_NONBLOCK)
    int flags = fcntl(fd_, F_GETFL, 0);
    if (flags == -1)
    {
        throw std::system_error(errno, std::generic_category(), "ASocket: Failed to get FD flags");
    }
    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        throw std::system_error(errno, std::generic_category(), "ASocket: Failed to set FD non-blocking");
    }

    // Set file descriptor flags (O_CLOEXEC)
    int fdFlags = fcntl(fd_, F_GETFD, 0);
    if (fdFlags == -1)
    {
        throw std::system_error(errno, std::generic_category(), "ASocket: Failed to get FD descriptor flags");
    }
    if (fcntl(fd_, F_SETFD, fdFlags | FD_CLOEXEC) == -1)
    {
        throw std::system_error(errno, std::generic_category(), "ASocket: Failed to set FD close-on-exec");
    }

    // Debug output
    flags = fcntl(fd_, F_GETFL, 0);
    fdFlags = fcntl(fd_, F_GETFD, 0);

    std::string flagStr = "0x" + std::to_string(flags) + " (";
    int mode = flags & 3;
    switch (mode)
    {
    case 0: flagStr += "O_RDONLY "; break;
    case 1: flagStr += "O_WRONLY "; break;
    case 2: flagStr += "O_RDWR "; break;
    default: flagStr += "UNKNOWN_MODE "; break;
    }
    if ((flags & O_NONBLOCK) != 0)
    {
        flagStr += "O_NONBLOCK ";
    }
    if ((fdFlags & FD_CLOEXEC) != 0)
    {
        flagStr += "FD_CLOEXEC ";
    }
    flagStr += ")";

    Log::debug(this->toString()  + " configured. Flags: " + flagStr);
}

int ASocket::getFd() const noexcept
{
    return fd_;
}

ASocket::IoState ASocket::getEvent() const noexcept
{
    return ioState_;
}

bool ASocket::isDirty() const noexcept
{
    return dirty_;
}

void ASocket::setIOState(IoState event)
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
    Log::debug(this->toString() + " processed");
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

std::string ASocket::toString() const
{
    return "ASocket(fd=" + std::to_string(fd_) + ")";
}