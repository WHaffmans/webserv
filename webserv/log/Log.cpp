#include <webserv/log/FileChannel.hpp>
#include <webserv/log/Log.hpp>
#include <webserv/log/StdoutChannel.hpp>

#include <chrono>
#include <filesystem>
#include <iostream>

Log::Log()
{
    // get start time
    start_time_ = std::chrono::steady_clock::now();
    channels_.insert({"stdout", std::unique_ptr<Channel>(new StdoutChannel())});
}

void Log::setFile(const std::string &filename)
{
    Log &log = getInstance();
    if (log.channels_.contains("file"))
    {
        log.channels_.erase("file");
    }
    try
    {
        log.channels_.insert({"file", std::unique_ptr<Channel>(new FileChannel(filename))});
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to set log file: " << e.what() << '\n';
    }
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
              const std::string &function, const std::string &channel,
              const std::map<std::string, std::string> &context)
{
    auto it = channels_.find(channel);
    if (it != channels_.end())
    {
        std::string extendedMessage;
        extendedMessage += message + " | ";
        if (!file.empty())
        {
            extendedMessage += std::filesystem::path(file).filename().string();
        }
        if (line != -1)
        {
            extendedMessage += ":" + std::to_string(line);
        }
        if (!function.empty())
        {
            extendedMessage += " (" + function + ")";
        }
        // extendedMessage += " | " + message;
        it->second->log(level, extendedMessage, context);
    }
}

int Log::getElapsedTime()
{
    Log &log = Log::getInstance();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - log.start_time_).count();
    return static_cast<int>(elapsed);
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