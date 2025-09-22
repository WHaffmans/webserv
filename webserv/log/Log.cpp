#include <webserv/log/FileChannel.hpp>
#include <webserv/log/Log.hpp>
#include <webserv/log/StdoutChannel.hpp>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <source_location>

Log::Log()
{
    // get start time
    start_time_ = std::chrono::steady_clock::now();
}

void Log::setStdoutChannel(Log::Level logLevel)
{
    Log &log = getInstance();
    if (log.channels_.contains("stdout"))
    {
        log.channels_.erase("stdout");
    }
    try
    {
        log.channels_.insert({"stdout", std::unique_ptr<Channel>(new StdoutChannel(logLevel))});
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to set stdout log channel: " << e.what() << '\n';
    }
}

void Log::setFileChannel(const std::string &filename, std::ios_base::openmode mode, Log::Level logLevel)
{
    Log &log = getInstance();
    if (log.channels_.contains("file"))
    {
        log.channels_.erase("file");
    }
    try
    {
        log.channels_.insert({"file", std::unique_ptr<Channel>(new FileChannel(filename, mode, logLevel))});
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

void Log::log(Log::Level level, const std::string &message, const std::map<std::string, std::string> &context,
              const std::source_location &location)
{
    for (auto &it : channels_)
    {
        std::string extendedMessage;
        extendedMessage += message + "\n\t| ";

        extendedMessage += std::filesystem::path(location.file_name()).filename().string();
        extendedMessage += ":" + std::to_string(location.line()) + ":" + std::to_string(location.column());
        extendedMessage += " (" + std::string(location.function_name()) + ")";

        // extendedMessage += " | " + message;
        it.second->log(level, extendedMessage, context);
    }
}

int Log::getElapsedTime()
{
    Log &log = Log::getInstance();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - log.start_time_).count();
    return static_cast<int>(elapsed);
}

void Log::trace(const std::string &message, const std::map<std::string, std::string> &context,
                const std::source_location &location)
{
    getInstance().log(Log::Level::Trace, message, context, location);
}

void Log::debug(const std::string &message, const std::map<std::string, std::string> &context,
                const std::source_location &location)
{
    getInstance().log(Log::Level::Debug, message, context, location);
}

void Log::info(const std::string &message, const std::map<std::string, std::string> &context,
               const std::source_location &location)
{
    getInstance().log(Log::Level::Info, message, context, location);
}

void Log::warning(const std::string &message, const std::map<std::string, std::string> &context,
                  const std::source_location &location)
{
    getInstance().log(Log::Level::Warn, message, context, location);
}

void Log::error(const std::string &message, const std::map<std::string, std::string> &context,
                const std::source_location &location)
{
    getInstance().log(Log::Level::Error, message, context, location);
}

void Log::fatal(const std::string &message, const std::map<std::string, std::string> &context,
                const std::source_location &location)
{
    getInstance().log(Log::Level::Fatal, message, context, location);
}

std::string Log::logLevelToString(Log::Level level)
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

const char *Log::logLevelToColor(Log::Level level)
{
    for (const auto &mapping : LOG_LEVEL_MAP)
    {
        if (mapping.level == level)
        {
            return mapping.color;
        }
    }
    return "\033[0m"; // Default to reset
}

std::string Log::logLevelToColoredString(Log::Level level)
{
    return std::string(Log::logLevelToColor(level)) + Log::logLevelToString(level) + "\033[0m";
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
    return Log::Level::Info; // Default fallback
}