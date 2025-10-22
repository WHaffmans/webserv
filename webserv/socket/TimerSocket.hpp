#pragma once

#include "webserv/socket/ASocket.hpp"

#include <chrono>

class TimerSocket : public ASocket
{
  public:
    explicit TimerSocket(std::chrono::milliseconds timeout);

    [[nodiscard]] ASocket::Type getType() const noexcept override;
    [[nodiscard]] bool isActive() const noexcept;

    ssize_t read(void *buf, size_t len) const override;
    ssize_t write(const void *buf, size_t len) const override;

    void activate();

  private:
    bool active_ = false;
    std::chrono::milliseconds timeout_;
};