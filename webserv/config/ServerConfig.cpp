#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/config/utils.hpp>
#include <webserv/log/Log.hpp>

#include <sstream>
#include <string>

ServerConfig::ServerConfig(std::string const &serverBlock) : port_(80)
{
    parseServerBlock(serverBlock);
}

void ServerConfig::parseServerBlock(const std::string &block)
{
    // Placeholder for actual server block parsing logic
    Log::trace(LOCATION);

    // Placeholder for actual file parsing logic

    std::string serverDeclarations;

    size_t pos = 0;
    while (true)
    {
        size_t locationPos = block.find("location", pos);

        size_t bracePos = block.find('{', locationPos);
        if (locationPos == std::string::npos || bracePos == std::string::npos)
        {
            // No more server blocks, remaining is global
            serverDeclarations += block.substr(pos);
            break;
        }
        std::string locationPath = trim(block.substr(locationPos, bracePos - (locationPos)));
        // Add global declarations before this server block
        serverDeclarations += block.substr(pos, locationPos - pos);
        size_t closeBrace = findCorrespondingClosingBrace(block, bracePos);
        if (closeBrace == std::string::npos)
        {
            throw std::runtime_error("Malformed block in config file.");
        }
        // Optionally parse the server block here
        std::string locationBlock = block.substr(bracePos + 1, closeBrace - bracePos - 1);
        locations_.emplace(locationPath, locationBlock);
        Log::debug("Added location: " + locationPath, {{"block", locationBlock}});
        pos = closeBrace + 1;
    }

    // parseGlobalDeclarations(Declarations); // Implement this function to handle global config
    parseDirectives(serverDeclarations);
}

void ServerConfig::parseDirectives(const std::string &declarations)
{
    Log::info("Parsing server directives");
    std::string line;
    std::istringstream stream(declarations);
    while (std::getline(stream, line))
    {
        std::string directive;
        std::istringstream lineStream{line};
        lineStream >> directive;
        if (!directive.empty())
        {
            std::string value;
            lineStream >> value;
            if (directive == "listen")
            {

                port_ = std::stoi(value);
                if (port_ < 1 || port_ > 65535)
                {
                    throw std::runtime_error("Invalid port number: " + std::to_string(port_));
                }
                Log::debug("Set port to " + std::to_string(port_));
            }
            else if (directive == "root")
            {
                root_ = value;
                Log::debug("Set root to " + root_);
            }
            else if (directive == "host")
            {
                host_ = value;
                Log::debug("Set host to " + host_);
            }
            else if (directive == "cgi_pass")
            {
                cgi_pass_ = value;
                Log::debug("Set cgi_pass to " + cgi_pass_);
            }
            else if (directive == "cgi_ext")
            {
                cgi_ext_ = value;
                Log::debug("Set cgi_ext to " + cgi_ext_);
            }
            else if (directive == "index")
            {
                index_files_.clear();
                std::string indexFile;
                while (lineStream >> indexFile)
                {
                    index_files_.push_back(indexFile);
                    Log::debug("Added index file: " + indexFile);
                }
            }
            else if (directive == "error_page")
            {
                int statusCode = std::stoi(value);
                std::string errorPagePath;
                lineStream >> errorPagePath;
                Log::debug("Set error_page for status " + std::to_string(statusCode) + " to " + errorPagePath);
            }
            else
            {
                Log::warning("Unknown directive: " + directive);
            }
        }
    }
}

const LocationConfig &ServerConfig::getLocation(const std::string &path) const
{
    if (locations_.count(path) > 0) // NOLINT
    {
        return locations_.at(path);
    }
    Log::error("Location not found: " + path);
    throw std::runtime_error("Location not found: " + path);
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