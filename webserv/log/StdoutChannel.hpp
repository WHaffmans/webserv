#pragma once

#include <webserv/log/Channel.hpp>

class StdoutChannel : public Channel
{
  public:
    StdoutChannel(LogLevel logLevel = LogLevel::LOGLVL_TRACE);

    StdoutChannel(const StdoutChannel &other) = delete;
    StdoutChannel(const StdoutChannel &&other) = delete;
    StdoutChannel &operator=(const StdoutChannel &other) = delete;
    StdoutChannel &&operator=(const StdoutChannel &&other) = delete;

    ~StdoutChannel();

    void log(LogLevel &logLevel, const std::string &message,
             const std::map<std::string, std::string> &context = {}) override;
};