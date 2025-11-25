#include <webserv/log/StdoutChannel.hpp>

#include <webserv/log/Log.hpp> // for Log

#include <iomanip>  // for operator<<, setfill, setw
#include <iostream> // for basic_ostream, operator<<, basic_ostream::operator<<, cerr, cout, flush, ostream

void StdoutChannel::log(const Log::Level &logLevel, const std::string &message,
                        const std::map<std::string, std::string> &context)
{

    std::ostream &out = (logLevel >= Log::Level::Warn) ? std::cerr : std::cout;
    out << "[" << std::setw(3) << std::setfill('0') << Log::getElapsedTime() << "] ";
    std::string prefix = "[" + Log::logLevelToColoredString(logLevel) + "] ";
    out << prefix;
    out << message << '\n';
    out << printContext(context);
    out << std::flush;
}

bool StdoutChannel::isStdOut() const
{
    return true;
}