#include <webserv/log/Channel.hpp>     // for Channel
#include <webserv/log/FileChannel.hpp> // for FileChannel
#include <webserv/log/Log.hpp>
#include <webserv/log/StdoutChannel.hpp> // for StdoutChannel

#include <chrono>    // for duration_cast, operator-, steady_clock, duration, seconds
#include <exception> // for exception
#include <iostream>  // for basic_ostream, operator<<, cerr
#include <memory>    // for allocator, unique_ptr, make_unique
#include <utility>   // for pair

Log::Log()
{
    // get start time
    start_time_ = std::chrono::steady_clock::now();
}

void Log::setStdoutChannel()
{
    Log &log = getInstance();
    if (log.channels_.contains("stdout"))
    {
        log.channels_.erase("stdout");
    }
    try
    {
        log.channels_["stdout"] = std::make_unique<StdoutChannel>();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to set stdout log channel: " << e.what() << '\n';
    }
}

void Log::setFileChannel(const std::string &filename, std::ios_base::openmode mode)
{
    Log &log = getInstance();
    if (log.channels_.contains("file"))
    {
        log.channels_.erase("file");
    }
    try
    {
        log.channels_["file"] = std::make_unique<FileChannel>(filename, mode);
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

void Log::log(Level level, const std::string &message, const std::map<std::string, std::string> &context)
{
    for (auto &it : channels_)
    {
        // extendedMessage += " | " + message;
        it.second->log(level, message, context);
    }
}

int Log::getElapsedTime()
{
    Log &log = Log::getInstance();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - log.start_time_).count();
    return static_cast<int>(elapsed);
}

void Log::trace(const std::string &message, const std::map<std::string, std::string> &context)
{
    if constexpr (COMPILE_TIME_LOG_LEVEL > Level::Trace)
    {
        return;
    }
    getInstance().log(Level::Trace, message, context);
}

void Log::debug(const std::string &message, const std::map<std::string, std::string> &context)
{
    if constexpr (COMPILE_TIME_LOG_LEVEL > Level::Debug)
    {
        return;
    }
    getInstance().log(Level::Debug, message, context);
}

void Log::info(const std::string &message, const std::map<std::string, std::string> &context)
{
    if constexpr (COMPILE_TIME_LOG_LEVEL > Level::Info)
    {
        return;
    }
    getInstance().log(Level::Info, message, context);
}

void Log::warning(const std::string &message, const std::map<std::string, std::string> &context)
{
    if constexpr (COMPILE_TIME_LOG_LEVEL > Level::Warn)
    {
        return;
    }
    getInstance().log(Level::Warn, message, context);
}

void Log::error(const std::string &message, const std::map<std::string, std::string> &context)
{
    if constexpr (COMPILE_TIME_LOG_LEVEL > Level::Error)
    {
        return;
    }
    getInstance().log(Level::Error, message, context);
}

void Log::fatal(const std::string &message, const std::map<std::string, std::string> &context)
{
    if constexpr (COMPILE_TIME_LOG_LEVEL > Level::Fatal)
    {
        return;
    }
    getInstance().log(Level::Fatal, message, context);
}

std::string Log::logLevelToString(Level level)
{

    for (const auto &mapping : LOG_LEVEL_MAP)
    {
        if (mapping.level == level)
        {
            return std::string(mapping.name);
        }
    }
    return "UNKNOWN";
}

const char *Log::logLevelToColor(Level level)
{
    for (const auto &mapping : LOG_LEVEL_MAP)
    {
        if (mapping.level == level)
        {
            return mapping.color;
        }
    }
    return RESET_COLOR; // Default to reset
}

std::string Log::logLevelToColoredString(Level level)
{
    return std::string(Log::logLevelToColor(level)) + Log::logLevelToString(level) + RESET_COLOR;
}

Log::Level Log::stringToLogLevel(const std::string &level)
{
    for (const auto &mapping : LOG_LEVEL_MAP)
    {
        if (mapping.name == level)
        {
            return mapping.level;
        }
    }
    return Level::Info; // Default fallback
}