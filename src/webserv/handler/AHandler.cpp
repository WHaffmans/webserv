#include <webserv/handler/AHandler.hpp> // for AHandler

#include <webserv/client/Client.hpp>    // for Client
#include <webserv/config/AConfig.hpp>   // for AConfig
#include <webserv/handler/URI.hpp>      // for URI
#include <webserv/http/HttpRequest.hpp> // for HttpRequest
#include <webserv/log/Log.hpp>          // for Log
#include <webserv/main.hpp>
#include <webserv/socket/TimerSocket.hpp> // for TimerSocket

#include <chrono>     // for operator*, milliseconds
#include <functional> // for function
#include <memory>     // for unique_ptr, make_unique
#include <optional>   // for optional
#include <string>     // for basic_string, operator+, to_string

AHandler::AHandler(const HttpRequest &request, HttpResponse &response) : request_(request), response_(response) {}

AHandler::~AHandler() = default;

void AHandler::startTimer()
{
    timerSocket_ = std::make_unique<TimerSocket>(
        std::chrono::milliseconds(request_.getUri().getConfig()->get<int>("timeout").value_or(DEFAULT_TIMEOUT)) * 1000);

    timerSocket_->setCallback([this]() { handleTimeout(); });
    timerSocket_->activate();

    request_.getClient().addSocket(timerSocket_.get());
    Log::debug("Timer started for handler: " + std::to_string(timerSocket_->getFd()));
}

TimerSocket *AHandler::getTimerSocket() const noexcept
{
    return timerSocket_.get();
}