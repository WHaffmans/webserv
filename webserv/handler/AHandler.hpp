#pragma once

#include "webserv/socket/TimerSocket.hpp"

#include <memory>
class HttpRequest;
class HttpResponse;

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

    void startTimer();
    void cancelTimer();

  protected:
    virtual void handleTimeout() = 0;

    const HttpRequest &request_;
    HttpResponse &response_;
    std::unique_ptr<TimerSocket> timerSocket_;
};