#include "webserv/log/Log.hpp"
#include <webserv/handler/AHandler.hpp>
#include <webserv/http/HttpRequest.hpp>
#include <webserv/http/HttpResponse.hpp>
#include <webserv/client/Client.hpp>

#include <memory>
#include <chrono>
#include <string>

AHandler::AHandler(const HttpRequest &request, HttpResponse &response) : request_(request), response_(response) {}

void AHandler::startTimer()
{
    timerSocket_ = std::make_unique<TimerSocket>(std::chrono::milliseconds(5000));

    timerSocket_->setCallback([this]() { handleTimeout(); });
    timerSocket_->activate();

    request_.getClient().addSocket(timerSocket_.get());
    Log::debug("Timer started for handler: " + std::to_string(timerSocket_->getFd()));
}

void AHandler::cancelTimer()
{
    if (timerSocket_)
    {
        request_.getClient().removeSocket(timerSocket_.get());
        timerSocket_ = nullptr;
    }
}