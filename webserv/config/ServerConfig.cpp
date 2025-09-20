#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/config/utils.hpp>
#include <webserv/log/Log.hpp>

#include <iostream>
#include <sstream>
#include <string>

ServerConfig::ServerConfig(std::string const &serverBlock) : port(80)
{
    parseServerBlock(serverBlock);
}

void ServerConfig::parseServerBlock(const std::string &block)
{
    // Placeholder for actual server block parsing logic
    LOG_INFO("Parsing server block...");

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
        LOG_INFO("Added location: " + locationPath);
        locations.emplace(locationPath, locationBlock);
        pos = closeBrace + 1;
    }

    // parseGlobalDeclarations(Declarations); // Implement this function to handle global config
    parseDirectives(serverDeclarations);
}

void ServerConfig::parseDirectives(const std::string &declarations)
{
    LOG_INFO("Parsing server directives" );
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

                port = std::stoi(value);
                if (port < 1 || port > 65535)
                {
                    throw std::runtime_error("Invalid port number: " + std::to_string(port));
                }
                LOG_INFO("Set port to " + std::to_string(port));
            }
            else if (directive == "root")
            {
                root = value;
                LOG_INFO("Set root to " + root);
            }
            else if (directive == "host")
            {
                host = value;
                LOG_INFO("Set host to " + host);
            }
            else if (directive == "cgi_pass")
            {
                cgi_pass = value;
                LOG_INFO("Set cgi_pass to " + cgi_pass);
            }
            else if (directive == "cgi_ext")
            {
                cgi_ext = value;
                LOG_INFO("Set cgi_ext to " + cgi_ext);
            }
            else if (directive == "index")
            {
                index_files.clear();
                std::string indexFile;
                while (lineStream >> indexFile)
                {
                    index_files.push_back(indexFile);
                    LOG_INFO("Added index file: " + indexFile);
                }
            }
            else if (directive == "error_page")
            {
                int statusCode = std::stoi(value);
                std::string errorPagePath;
                lineStream >> errorPagePath;
                error_page[statusCode] = errorPagePath;
                LOG_INFO("Set error_page for status " + std::to_string(statusCode) + " to " + errorPagePath);
            }
            else
            {
                LOG_WARN("Unknown directive: " + directive);
            }
        }
    }
}

const LocationConfig &ServerConfig::getLocation(const std::string &path) const
{
    if (locations.count(path) > 0) // NOLINT
    {
        return locations.at(path);
    }
    LOG_ERROR("Location not found: " + path);
    throw std::runtime_error("Location not found: " + path);
}

std::vector<std::string> ServerConfig::getLocationPaths() const
{
    std::vector<std::string> paths;
    paths.reserve(locations.size());
    for (const auto &pair : locations)
    {
        paths.push_back(pair.first);
    }
    return paths;
}