#include <webserv/log/Log.hpp> // for Log
#include <webserv/log/StdoutChannel.hpp>

#include <iomanip>  // for operator<<, setfill, setw
#include <iostream> // for basic_ostream, operator<<, basic_ostream::operator<<, cerr, cout, flush, ostream

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