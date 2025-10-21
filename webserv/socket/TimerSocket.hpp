#pragma once

#include "webserv/socket/ASocket.hpp"
#include <chrono>

class TimerSocket : public ASocket
{
    public:
        explicit TimerSocket(std::chrono::milliseconds timeout);
        
        [[nodiscard]] ASocket::Type getType() const noexcept override;
        [[nodiscard]] bool isActive() const noexcept;

        void activate();
    private:
        bool active_ = false;
        std::chrono::milliseconds timeout_;
};