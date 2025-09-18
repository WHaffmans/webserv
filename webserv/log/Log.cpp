#include "webserv/log/StdoutChannel.hpp"
#include <webserv/log/Log.hpp>

Log::Log()
{
    channels_.insert({"stdout", std::unique_ptr<Channel>(new StdoutChannel())});
}

Log &Log::getInstance()
{
    static Log instance;

    return instance;
}

void Log::log(LogLevel level, const std::string &message, const std::string &channel,
              const std::map<std::string, std::string> &context)
{
    auto it = channels_.find(channel);
    if (it != channels_.end())
    {
        it->second->log(level, message, context);
    }
}

void Log::debug(const std::string &message, const std::map<std::string, std::string> &context)
{
    getInstance().log(LogLevel::DEBUG, message, "stdout", context);
}

void Log::info(const std::string &message, const std::map<std::string, std::string> &context)
{
    getInstance().log(LogLevel::INFO, message, "stdout", context);
}

void Log::warning(const std::string &message, const std::map<std::string, std::string> &context)
{
    getInstance().log(LogLevel::WARN, message, "stdout", context);
}

void Log::error(const std::string &message, const std::map<std::string, std::string> &context)
{
    getInstance().log(LogLevel::ERROR, message, "stdout", context);
}