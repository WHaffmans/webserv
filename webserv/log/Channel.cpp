#include <webserv/log/Channel.hpp>

Log::Level Channel::getLogLevel() const
{
    return logLevel_;
}

void Channel::setLogLevel(Log::Level level)
{
    logLevel_ = level;
}