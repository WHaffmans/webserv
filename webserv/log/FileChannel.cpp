#include <webserv/log/FileChannel.hpp>

#include <webserv/log/Log.hpp> // for Log

#include <chrono>   // for system_clock
#include <ctime>    // for localtime, tm
#include <iomanip>  // for operator<<, put_time
#include <iostream> // for cerr

struct tm;

FileChannel::FileChannel(const std::string &filename) : filename_(filename), fileStream_(filename, std::ios_base::trunc)
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

void FileChannel::log(const Log::Level &logLevel, const std::string &message,
                      const std::map<std::string, std::string> &context)
{

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
                << "[" << Log::logLevelToString(logLevel) << "] " << message << '\n';

    // Log the context if it exists
    fileStream_ << printContext(context);
    fileStream_ << std::flush;
}
