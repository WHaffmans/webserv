#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/config/utils.hpp>

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
    std::cout << "Parsing server block:\n";

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
        std::string serverBlock = block.substr(bracePos + 1, closeBrace - bracePos - 1);
        locations.emplace(locationPath, LocationConfig(serverBlock));
        pos = closeBrace + 1;
    }

    // parseGlobalDeclarations(Declarations); // Implement this function to handle global config
    parseDirectives(serverDeclarations);
}

void ServerConfig::parseDirectives(const std::string &declarations)
{
    // Placeholder for actual directives parsing logic
    std::cout << "Parsing directives:\n" << declarations << '\n';
    std::string line;
    std::istringstream stream(declarations);
    while (std::getline(stream, line))
    {
        std::string directive;
        std::istringstream lineStream{trim(line)};
        lineStream >> directive;
        if (!directive.empty())
        {
            std::cout << "Directive: " << directive << '\n';
            // Implement the parsing logic here
            std::string value;
            lineStream >> value;
            if (directive == "listen")
            {

                port = std::stoi(value);
                if (port < 1 || port > 65535)
                {
                    throw std::runtime_error("Invalid port number: " + std::to_string(port));
                }
                std::cout << "Set port to " << port << '\n';
            }
            else if (directive == "root")
            {
                root = value;
                std::cout << "Set root to " << root << '\n';
            }
            else if (directive == "server_name")
            {
                host = value;
                std::cout << "Set server_name to " << host << '\n';
            }
            else if (directive == "cgi_pass")
            {
                cgi_pass = value;
                std::cout << "Set cgi_pass to " << cgi_pass << '\n';
            }
            else if (directive == "cgi_ext")
            {
                cgi_ext = value;
                std::cout << "Set cgi_ext to " << cgi_ext << '\n';
            }
            else if (directive == "index")
            {
                index_files.clear();
                std::string indexFile;
                while (lineStream >> indexFile)
                {
                    index_files.push_back(indexFile);
                    std::cout << "Added index file: " << indexFile << '\n';
                }
            }
            else if (directive == "error_page")
            {
                int statusCode = std::stoi(value);
                std::string errorPagePath;
                lineStream >> errorPagePath;
                error_page[statusCode] = errorPagePath;
                std::cout << "Set error_page for status " << statusCode << " to " << errorPagePath << '\n';
            }
        }
    }
}