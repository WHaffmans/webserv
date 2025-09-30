#include <webserv/config/AConfig.hpp> // for AConfig
#include <webserv/config/LocationConfig.hpp>
#include <webserv/log/Log.hpp> // for Log, LOCATION

LocationConfig::LocationConfig(const std::string &block, const std::string &path, const AConfig *parent)
    : AConfig(parent), _path(path)
{
    parseBlock(block);
}

void LocationConfig::parseBlock(const std::string &block)
{
    Log::trace(LOCATION);
    parseDirectives(block);
}