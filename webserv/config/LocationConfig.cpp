#include <webserv/config/AConfig.hpp> // for AConfig
#include <webserv/config/LocationConfig.hpp>

LocationConfig::LocationConfig(const std::string &block, const std::string &path, const AConfig *parent)
    : AConfig(parent), _path(path)
{
    parseBlock(block);
}

// std::string LocationConfig::getName() const
// {
//     auto parentName = parent_ != nullptr ? parent_->getName() : "root";
//     return parentName + ", location: " + _path;
// }

std::string LocationConfig::getType() const
{
    return "location";
}

void LocationConfig::parseBlock(const std::string &block)
{
    // Detect nested location blocks which are not allowed
    if (block.find("location") != std::string::npos)
    {
        throw std::runtime_error("Nested location blocks are not allowed (too many levels)");
    }
    parseDirectives(block);
}

const std::string &LocationConfig::getPath() const noexcept
{
    return _path;
}