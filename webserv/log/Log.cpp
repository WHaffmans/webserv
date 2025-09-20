#include "webserv/log/StdoutChannel.hpp"
#include <webserv/log/Log.hpp>
#include <filesystem>

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
void Log::log(LogLevel level, const std::string &message, const std::string &file, int line,
              const std::string &function, const std::string &channel, const std::map<std::string, std::string> &context)
{
    auto it = channels_.find(channel);
    if (it != channels_.end())
    {
std::string extendedMessage;
        if (!file.empty())
        {
            extendedMessage +=  std::filesystem::path(file).filename().string();
        }
        if (line != -1)
        {
            extendedMessage += ":" + std::to_string(line);
        }
        if (!function.empty())
        {
            extendedMessage += " (" + function + ")";
        }
        extendedMessage += " | " + message;
        it->second->log(level, extendedMessage, context);
    }
}

void Log::static_log(LogLevel level, const std::string &message, const std::string &file, int line,
                     const std::string &function, const std::string &channel,
                     const std::map<std::string, std::string> &context)
{
    getInstance().log(level, message, file, line, function, channel, context);
}

void Log::trace(const std::string &message, const std::map<std::string, std::string> &context)
{
    getInstance().log(LogLevel::LOGLVL_TRACE, message, "stdout", context);
}

void Log::debug(const std::string &message, const std::map<std::string, std::string> &context)
{
    getInstance().log(LogLevel::LOGLVL_DEBUG, message, "stdout", context);
}

void Log::info(const std::string &message, const std::map<std::string, std::string> &context)
{
    getInstance().log(LogLevel::LOGLVL_INFO, message, "stdout", context);
}

void Log::warning(const std::string &message, const std::map<std::string, std::string> &context)
{
    getInstance().log(LogLevel::LOGLVL_WARN, message, "stdout", context);
}

void Log::error(const std::string &message, const std::map<std::string, std::string> &context)
{
    getInstance().log(LogLevel::LOGLVL_ERROR, message, "stdout", context);
}

void Log::fatal(const std::string &message, const std::map<std::string, std::string> &context)
{
    getInstance().log(LogLevel::LOGLVL_FATAL, message, "stdout", context);
}