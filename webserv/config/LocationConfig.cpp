#include <webserv/config/LocationConfig.hpp>

#include <webserv/config/AConfig.hpp> // for AConfig
#include <webserv/log/Log.hpp> // for Log, LOCATION

LocationConfig::LocationConfig(const std::string &block, const std::string &path, const AConfig *parent)
    : AConfig(parent), _path(path)
{
    parseBlock(block);
}

std::string LocationConfig::getName() const
{
    auto parentName = parent_ != nullptr ? parent_->getName() : "root";
    return parentName + ", location: " + _path;
}

std::string LocationConfig::getType() const
{
    return "location";
}

void LocationConfig::parseBlock(const std::string &block)
{
    Log::trace(LOCATION);
    parseDirectives(block);
}