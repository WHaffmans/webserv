#pragma once

#include <webserv/socket/ASocket.hpp> // for ASocket

#include <chrono> // for milliseconds
#include <string>

#include <stddef.h>    // for size_t
#include <sys/types.h> // for ssize_t

class TimerSocket : public ASocket
{
  public:
    explicit TimerSocket(std::chrono::milliseconds timeout);

    [[nodiscard]] ASocket::Type getType() const noexcept override;
    [[nodiscard]] bool isActive() const noexcept;

    ssize_t read(void *buf, size_t len) const override;
    ssize_t write(const void *buf, size_t len) const override;

    void activate();

    [[nodiscard]] std::string toString() const override;

  private:
    bool active_ = false;
    std::chrono::milliseconds timeout_;
};