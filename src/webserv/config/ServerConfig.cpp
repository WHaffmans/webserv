#include <webserv/config/ServerConfig.hpp>

#include <webserv/config/AConfig.hpp>        // for AConfig
#include <webserv/config/LocationConfig.hpp> // for LocationConfig
#include <webserv/log/Log.hpp>     // for Log, LOCATION
#include <webserv/utils/utils.hpp> // for findCorrespondingClosingBrace, trim

#include <optional>  // for optional
#include <stdexcept> // for runtime_error
#include <utility>   // for pair

#include <stddef.h> // for size_t

ServerConfig::ServerConfig(const std::string &block, const AConfig *parent) : AConfig(parent)
{
    parseBlock(block);
}

std::string ServerConfig::getType() const
{
    return "server";
}

void ServerConfig::parseBlock(const std::string &block)
{
    Log::trace(LOCATION);

    std::string directives;

    size_t pos = 0;
    std::string location = "location";
    while (true)
    {
        size_t locationPos = block.find(location, pos);

        size_t bracePos = block.find('{', locationPos);
        if (locationPos == std::string::npos || bracePos == std::string::npos)
        {
            // No more server blocks, remaining is global
            directives += block.substr(pos);
            break;
        }
        std::string locationPath = utils::trim(
            block.substr(locationPos + location.size() + 1, bracePos - (locationPos + location.size() + 1)));
        // Add global declarations before this server block

        if (locationPath.front() != '/')
        {
            throw std::runtime_error("Location path must start with '/': " + locationPath);
        }

        directives += block.substr(pos, locationPos - pos);
        size_t closeBrace = utils::findCorrespondingClosingBrace(block, bracePos);
        if (closeBrace == std::string::npos)
        {
            throw std::runtime_error("Malformed block in config file.");
        }
        // Optionally parse the server block here
        std::string locationBlock = block.substr(bracePos + 1, closeBrace - bracePos - 1);
        if (locations_.contains(locationPath))
        {
            throw std::runtime_error("Conflicting location block: " + locationPath);
        }
        locations_[locationPath] = std::make_unique<LocationConfig>(locationBlock, locationPath, this);
        Log::debug("Added location: " + locationPath, {{"block", locationBlock}});
        pos = closeBrace + 1;
    }

    // parseGlobalDeclarations(Declarations); // Implement this function to handle global config
    // Detect unexpected nested blocks like 'http { ... }' in server context
    if (directives.find('{') != std::string::npos || directives.find('}') != std::string::npos)
    {
        throw std::runtime_error("Invalid block type in server context (http block not allowed)");
    }
    parseDirectives(directives);
}

const LocationConfig *ServerConfig::getLocation(const std::string &path) const
{
    auto it = locations_.find(path);
    return (it != locations_.end()) ? it->second.get() : nullptr;
}

std::vector<std::string> ServerConfig::getLocationPaths() const
{
    std::vector<std::string> paths;
    paths.reserve(locations_.size());
    for (const auto &pair : locations_)
    {
        paths.push_back(pair.first);
    }
    return paths;
}
