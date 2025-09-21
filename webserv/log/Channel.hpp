#pragma once

#include <webserv/log/Log.hpp>
#include <webserv/log/LogLevel.hpp>

#include <map>
#include <string>

class Channel
{
  public:
    Channel() = default;
    Channel(LogLevel logLevel);
    virtual ~Channel() = default;

    Channel(const Channel &other) = delete;
    Channel(const Channel &&other) = delete;
    Channel &operator=(const Channel &other) = delete;
    Channel &&operator=(const Channel &&other) = delete;

    virtual void log(LogLevel &logLevel, const std::string &message,
                     const std::map<std::string, std::string> &context = {}) = 0;

  protected:
    LogLevel logLevel_{LogLevel::LOGLVL_TRACE};
};