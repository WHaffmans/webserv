#pragma once

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <webserv/log/Channel.hpp>
#include <webserv/log/LogLevel.hpp>

#define LOG(level, message) Log::static_log((level), (message), __FILE__, __LINE__, __FUNCTION__, "file", {})

#define LOG_TRACE(message) LOG(LogLevel::LOGLVL_TRACE, message)
#define LOG_INFO(message) LOG(LogLevel::LOGLVL_INFO, message)
#define LOG_DEBUG(message) LOG(LogLevel::LOGLVL_DEBUG, message)
#define LOG_WARN(message) LOG(LogLevel::LOGLVL_WARN, message)
#define LOG_ERROR(message) LOG(LogLevel::LOGLVL_ERROR, message)
#define LOG_FATAL(message) LOG(LogLevel::LOGLVL_FATAL, message)

class Channel; // Forward declaration

class Log
{
  public:
    Log(const Log &other) = delete;
    Log(const Log &&other) = delete;
    Log &operator=(const Log &other) = delete;
    Log &&operator=(const Log &&other) = delete;

    static void setFile(const std::string &filename);

    void log(LogLevel level, const std::string &message, const std::string &channel = "stdout",
             const std::map<std::string, std::string> &context = {});

    void log(LogLevel level, const std::string &message, const std::string &file = "", int line = -1,
             const std::string &function = "", const std::string &channel = "stdout",
             const std::map<std::string, std::string> &context = {});

    static int getElapsedTime();

    static void static_log(LogLevel level, const std::string &message, const std::string &file = "", int line = -1,
                           const std::string &function = "", const std::string &channel = "stdout",
                           const std::map<std::string, std::string> &context = {});

    static void trace(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void debug(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void info(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void warning(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void error(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void fatal(const std::string &message, const std::map<std::string, std::string> &context = {});

  private:
    Log();

    ~Log() = default;
    static Log &getInstance();

    std::chrono::steady_clock::time_point start_time_;
    std::unordered_map<std::string, std::unique_ptr<Channel>> channels_;
};