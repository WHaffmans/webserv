#include "webserv/log/Log.hpp"
#include "webserv/socket/ASocket.hpp"

#include <webserv/socket/TimerSocket.hpp>

#include <stdexcept>

#include <sys/time.h>
#include <sys/timerfd.h>

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