#pragma once

#include <webserv/log/Log.hpp>


#include <map>
#include <string>

class Channel
{
  public:
    Channel() = default;
    Channel(Log::Level logLevel);
    virtual ~Channel() = default;

    Channel(const Channel &other) = delete;
    Channel(const Channel &&other) = delete;
    Channel &operator=(const Channel &other) = delete;
    Channel &&operator=(const Channel &&other) = delete;

    virtual void log(Log::Level &logLevel, const std::string &message,
                     const std::map<std::string, std::string> &context = {}) = 0;

  protected:
    Log::Level logLevel_{Log::Level::Trace};
};