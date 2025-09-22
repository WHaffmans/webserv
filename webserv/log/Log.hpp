#pragma once

#include <webserv/log/Channel.hpp>
#include <webserv/log/LogLevel.hpp>

#include <chrono>
#include <map>
#include <memory>
#include <source_location>
#include <string>
#include <unordered_map>
/*
TODO ACTUALLY WE USE C++20 SO WE CAN USE std::source_location TO AUTOMATICALLY CAPTURE FILE, LINE, FUNCTION

#include <source_location>

void log(LogLevel level, const std::string& message,
         std::source_location location = std::source_location::current()) {
    std::cout << "File: " << location.file_name() << std::endl;
    std::cout << "Line: " << location.line() << std::endl;
    std::cout << "Function: " << location.function_name() << std::endl;
}

No macros needed!!!!
*/

class Channel; // Forward declaration

class Log
{
  public:
    Log(const Log &other) = delete;
    Log(const Log &&other) = delete;
    Log &operator=(const Log &other) = delete;
    Log &&operator=(const Log &&other) = delete;

    void log(LogLevel level, const std::string &message, const std::map<std::string, std::string> &context,
             const std::source_location &location);

    static void setFileChannel(const std::string &filename, std::ios_base::openmode mode = std::ios_base::app,
                               LogLevel logLevel = LogLevel::LOGLVL_TRACE);
    static void setStdoutChannel(LogLevel logLevel = LogLevel::LOGLVL_TRACE);

    static int getElapsedTime();

    static void trace(const std::string &message, const std::map<std::string, std::string> &context = {},
                      const std::source_location &location = std::source_location::current());
    static void debug(const std::string &message, const std::map<std::string, std::string> &context = {},
                      const std::source_location &location = std::source_location::current());
    static void info(const std::string &message, const std::map<std::string, std::string> &context = {},
                     const std::source_location &location = std::source_location::current());
    static void warning(const std::string &message, const std::map<std::string, std::string> &context = {},
                        const std::source_location &location = std::source_location::current());
    static void error(const std::string &message, const std::map<std::string, std::string> &context = {},
                      const std::source_location &location = std::source_location::current());
    static void fatal(const std::string &message, const std::map<std::string, std::string> &context = {},
                      const std::source_location &location = std::source_location::current());

  private:
    Log();

    ~Log() = default;
    static Log &getInstance();

    std::chrono::steady_clock::time_point start_time_;
    std::unordered_map<std::string, std::unique_ptr<Channel>> channels_;
};