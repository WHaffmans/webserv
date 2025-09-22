#pragma once

#include <webserv/log/Log.hpp>

#include <map>
#include <string>

class Channel
{
  public:
    Channel() = default;
    virtual ~Channel() = default;
    Channel(const Channel &other) = delete;
    Channel(Channel &&other) = delete;
    Channel &operator=(const Channel &other) = delete;
    Channel &operator=(Channel &&other) = delete;
    virtual void log(const Log::Level &logLevel, const std::string &message,
                     const std::map<std::string, std::string> &context = {}) = 0;

  protected:
    [[nodiscard]] Log::Level getLogLevel() const;
    void setLogLevel(Log::Level level);

  private:
    Log::Level logLevel_{Log::Level::Trace};
};