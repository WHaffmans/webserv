#pragma once

#include <webserv/log/Channel.hpp>

class StdoutChannel : public Channel
{
  public:
    void log(LogLevel &logLevel, const std::string &message,
             const std::map<std::string, std::string> &context = {}) override;
};