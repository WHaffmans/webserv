#include <webserv/socket/TimerSocket.hpp>

#include <webserv/log/Log.hpp>        // for LOCATION, Log
#include <webserv/socket/ASocket.hpp> // for ASocket

#include <stdexcept>    // for runtime_error
#include <system_error> // for generic_category, system_error

#include <errno.h>       // for errno
#include <sys/time.h>    // for CLOCK_MONOTONIC
#include <sys/timerfd.h> // for timerfd_create, timerfd_settime
#include <time.h>        // for itimerspec, timespec
#include <unistd.h>      // for read, write

TimerSocket::TimerSocket(std::chrono::milliseconds timeout)
    : ASocket(timerfd_create(CLOCK_MONOTONIC, 0), ASocket::IoState::NONE), timeout_(timeout)
{
}

void TimerSocket::activate()
{
    Log::trace(LOCATION);
    struct itimerspec timerSpec;

    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(timeout_);
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(timeout_ - seconds);

    timerSpec.it_value.tv_sec = seconds.count();
    timerSpec.it_value.tv_nsec = nanoseconds.count();
    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = 0;

    if (timerfd_settime(getFd(), 0, &timerSpec, nullptr) == -1)
    {
        throw std::runtime_error("Failed to set timerfd time");
    }

    active_ = true;
    setIOState(IoState::READ);
}

ASocket::Type TimerSocket::getType() const noexcept
{
    return ASocket::Type::TIMER_SOCKET;
}

bool TimerSocket::isActive() const noexcept
{
    return active_;
}

ssize_t TimerSocket::read(void *buf, size_t len) const
{
    Log::trace(LOCATION);
    ssize_t bytesRead = ::read(getFd(), buf, len);
    if (bytesRead == -1)
    {
        throw std::system_error(errno, std::generic_category(), "Socket: Read error");
    }
    return bytesRead;
}

ssize_t TimerSocket::write(const void *buf, size_t len) const
{
    Log::trace(LOCATION);
    ssize_t bytesSent = ::write(getFd(), buf, len);
    if (bytesSent == -1)
    {
        throw std::system_error(errno, std::generic_category(), "Socket: Write error");
    }
    return bytesSent;
}