#include "webserv/log/StdoutChannel.hpp"

#include <webserv/log/Channel.hpp>
#include <webserv/log/Log.hpp>


#include <iomanip>
#include <iostream>
#include <map>

StdoutChannel::StdoutChannel(Log::Level logLevel) : Channel(logLevel) {}

StdoutChannel::~StdoutChannel() {}

void StdoutChannel::log(Log::Level &logLevel, const std::string &message,
                        const std::map<std::string, std::string> &context)
{
    if (logLevel < logLevel_)
    {
        return;
    }
    std::cout << "[" << std::setw(3) << std::setfill('0') << Log::getElapsedTime() << "] ";
    std::string prefix = "[" + Log::logLevelToColoredString(logLevel) + "] ";
    std::cout << prefix;
    std::cout << message;
    if (!context.empty())
    {
        std::cout << " Context: {";
        for (const auto &[key, value] : context)
        {
            std::cout << key << ": " << value << ", ";
        }
        std::cout << "}";
    }
    std::cout << "\n";
}