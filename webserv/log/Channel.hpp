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
                     const std::map<std::string, std::string> &context = {})
        = 0;

    [[nodiscard]] virtual bool isStdOut() const;
  protected:
    [[nodiscard]] static std::string printContext(const std::map<std::string, std::string> &context);

  private:
};