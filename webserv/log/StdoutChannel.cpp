#include <webserv/log/Channel.hpp>
#include <webserv/log/Log.hpp>
#include <webserv/log/StdoutChannel.hpp>

#include <iomanip>
#include <iostream>
#include <map>
#include <ostream>

StdoutChannel::StdoutChannel(Log::Level logLevel)
{
    setLogLevel(logLevel);
}

void StdoutChannel::log(const Log::Level &logLevel, const std::string &message,
                        const std::map<std::string, std::string> &context)
{
    if (logLevel < getLogLevel())
    {
        return;
    }
    std::ostream &out = (logLevel >= Log::Level::Warn) ? std::cerr : std::cout;
    out << "[" << std::setw(3) << std::setfill('0') << Log::getElapsedTime() << "] ";
    std::string prefix = "[" + Log::logLevelToColoredString(logLevel) + "] ";
    out << prefix;
    out << message << '\n';
    out << printContext(context);
    out << std::flush;
}