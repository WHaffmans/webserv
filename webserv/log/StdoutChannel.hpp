#pragma once

#include <webserv/log/Channel.hpp>

class StdoutChannel : public Channel
{
  public:
    StdoutChannel(Log::Level logLevel = Log::Level::Trace);

    StdoutChannel(const StdoutChannel &other) = delete;
    StdoutChannel(const StdoutChannel &&other) = delete;
    StdoutChannel &operator=(const StdoutChannel &other) = delete;
    StdoutChannel &&operator=(const StdoutChannel &&other) = delete;

    ~StdoutChannel();

    void log(Log::Level &logLevel, const std::string &message,
             const std::map<std::string, std::string> &context = {}) override;
};