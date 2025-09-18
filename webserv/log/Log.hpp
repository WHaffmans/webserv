#pragma once

#include "webserv/log/Channel.hpp"
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <webserv/log/LogLevel.hpp>


#define LOG_INFO(message) \
  Log::info(message, std::map<std::string, std::string>{{"file", __FILE__}, {"line", std::to_string(__LINE__)}, {"function", __FUNCTION__}})

class Channel; // Forward declaration

class Log
{
  public:
    Log(const Log &other) = delete;
    Log(const Log &&other) = delete;
    Log &operator=(const Log &other) = delete;
    Log &&operator=(const Log &&other) = delete;

    void log(LogLevel level, const std::string &message, const std::string &channel = "stdout",
             const std::map<std::string, std::string> &context = {});

    static void debug(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void info(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void warning(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void error(const std::string &message, const std::map<std::string, std::string> &context = {});

  private:
    Log();
    ~Log() = default;
    static Log &getInstance();

    std::unordered_map<std::string, std::unique_ptr<Channel>> channels_;
};