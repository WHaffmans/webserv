#include <webserv/config/AConfig.hpp> // for AConfig
#include <webserv/config/LocationConfig.hpp>
#include <webserv/log/Log.hpp> // for Log, LOCATION

LocationConfig::LocationConfig(const std::string &block, const AConfig *parent) : AConfig(parent)
{
    parseBlock(block);
}

void LocationConfig::parseBlock(const std::string &block)
{
    Log::trace(LOCATION);
    parseDirectives(block);
}