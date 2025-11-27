#pragma once

#include <webserv/socket/TimerSocket.hpp>

#include <memory> // for unique_ptr

class HttpRequest;
class HttpResponse;
class TimerSocket;

class AHandler
{
  public:
    AHandler(const HttpRequest &request, HttpResponse &response);
    virtual ~AHandler();

    AHandler(const AHandler &other) = delete;
    AHandler &operator=(const AHandler &other) = delete;
    AHandler(AHandler &&other) noexcept = delete;
    AHandler &operator=(AHandler &&other) noexcept = delete;

    virtual void handle() = 0;

    virtual void startTimer();

    [[nodiscard]] TimerSocket *getTimerSocket() const noexcept;

  protected:
    virtual void handleTimeout() = 0;

    const HttpRequest &request_;
    HttpResponse &response_;
    std::unique_ptr<TimerSocket> timerSocket_;
};