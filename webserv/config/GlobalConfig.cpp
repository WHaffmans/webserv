#include <webserv/config/GlobalConfig.hpp>

#include <webserv/log/Log.hpp>     // for Log
#include <webserv/utils/utils.hpp> // for findCorrespondingClosingBrace

#include <stdexcept> // for runtime_error
#include <vector>    // for vector

#include <stddef.h> // for size_t

GlobalConfig::GlobalConfig(const std::string &block)
{
    parseBlock(block);
}

std::string GlobalConfig::getName() const
{
    return "global";
}

std::string GlobalConfig::getType() const
{
    return "global";
}

void GlobalConfig::parseBlock(const std::string &block)
{
    std::string directives;
    Log::trace("Content before parsing servers:\n" + block);
    size_t pos = 0;
    while (true)
    {
        size_t serverPos = block.find("server", pos);
        size_t bracePos = block.find('{', serverPos);
        if (serverPos == std::string::npos || bracePos == std::string::npos)
        {
            // No more server blocks, remaining is global
            directives += block.substr(pos);
            break;
        }
        // Add global declarations before this server block
        directives += block.substr(pos, serverPos - pos);
        size_t closeBrace = utils::findCorrespondingClosingBrace(block, bracePos);
        if (closeBrace == std::string::npos)
        {
            throw std::runtime_error("Malformed block in config file.");
        }
        // Optionally parse the server block here
        std::string serverBlock = block.substr(bracePos + 1, closeBrace - bracePos - 1);
        servers_.emplace_back(std::make_unique<ServerConfig>(serverBlock, this));
        pos = closeBrace + 1;
    }

    parseDirectives(directives);
}

std::vector<ServerConfig *> GlobalConfig::getServerConfigs() const
{
    std::vector<ServerConfig *> serverConfigs;
    serverConfigs.reserve(servers_.size());
    for (const auto &serverPtr : servers_)
    {
        serverConfigs.push_back(serverPtr.get());
    }
    return serverConfigs;
}