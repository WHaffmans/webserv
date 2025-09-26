#pragma once

#include <webserv/log/Channel.hpp> // for Channel
#include <webserv/log/Log.hpp>     // for Log

#include <map>    // for map
#include <string> // for string, basic_string

class StdoutChannel : public Channel
{
  public:
    StdoutChannel(Log::Level logLevel = Log::Level::Trace);

    StdoutChannel(const StdoutChannel &other) = delete;
    StdoutChannel(StdoutChannel &&other) = delete;
    StdoutChannel &operator=(const StdoutChannel &other) = delete;
    StdoutChannel &operator=(StdoutChannel &&other) = delete;

    ~StdoutChannel() override = default;

    void log(const Log::Level &logLevel, const std::string &message,
             const std::map<std::string, std::string> &context = {}) override;
};