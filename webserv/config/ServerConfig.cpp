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

// std::string ServerConfig::getName() const
// {
//     return "server: " + get<std::string>("server_name").value_or("unnamed") + " (port "
//            + std::to_string(get<int>("listen").value_or(-1)) + ")";
// }

std::string ServerConfig::getType() const
{
    return "server";
}

void ServerConfig::parseBlock(const std::string &block)
{
    Log::trace(LOCATION);

    std::string directives;

    size_t pos = 0;
    while (true)
    {
        size_t locationPos = block.find("location", pos);

        size_t bracePos = block.find('{', locationPos);
        if (locationPos == std::string::npos || bracePos == std::string::npos)
        {
            // No more server blocks, remaining is global
            directives += block.substr(pos);
            break;
        }
        std::string locationPath
            = utils::trim(block.substr(locationPos + 9, bracePos - (locationPos + 9))); // TODO magic numbers
        // Add global declarations before this server block
        directives += block.substr(pos, locationPos - pos);
        size_t closeBrace = utils::findCorrespondingClosingBrace(block, bracePos);
        if (closeBrace == std::string::npos)
        {
            throw std::runtime_error("Malformed block in config file.");
        }
        // Optionally parse the server block here
        std::string locationBlock = block.substr(bracePos + 1, closeBrace - bracePos - 1);
        locations_[locationPath] = std::make_unique<LocationConfig>(locationBlock, locationPath, this);
        Log::debug("Added location: " + locationPath, {{"block", locationBlock}});
        pos = closeBrace + 1;
    }

    // parseGlobalDeclarations(Declarations); // Implement this function to handle global config
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
