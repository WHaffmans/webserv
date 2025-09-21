#include "webserv/log/LogLevel.hpp"

#include <webserv/log/FileChannel.hpp>

#include <chrono>
#include <ios>
#include <iostream>

FileChannel::FileChannel(const std::string &filename, std::ios_base::openmode mode, LogLevel logLevel)
    : Channel(logLevel), filename_(filename), fileStream_(filename, mode)
{
    if (!fileStream_.is_open())
    {
        std::cerr << "Failed to open log file: " << filename << '\n';
    }
}

FileChannel::~FileChannel()
{
    if (fileStream_.is_open())
    {
        fileStream_.close();
    }
}

void FileChannel::log(LogLevel &logLevel, const std::string &message, const std::map<std::string, std::string> &context)
{
    if (logLevel < logLevel_)
    {
        return;
    }
    if (!fileStream_.is_open())
    {
        std::cerr << "Log file is not open: " << filename_ << '\n';
        return;
    }

    // Get the current time
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::tm *tm = std::localtime(&now_c);

    // Format the log message
    fileStream_ << "[" << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << "] "
                << "[" << logLevelToString(logLevel) << "] " << message << '\n';

    // Log the context if it exists
    if (!context.empty())
    {
        fileStream_ << "Context:" << '\n';
        for (const auto &[key, value] : context)
        {
            fileStream_ << "  " << key << ": " << value << '\n';
        }
    }
    fileStream_ << std::flush;
}
