#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/utils.hpp>


#include <iostream>
#include <string>
#include <sstream>

LocationConfig::LocationConfig(const std::string &locationBlock)
    : path(""), autoIndex(false), indexFile("")
{
    parseLocationBlock(locationBlock);
}

void LocationConfig::parseLocationBlock(const std::string &block)
{
    // Placeholder for actual location block parsing logic
    std::cout << "Parsing location block:\n" << block << '\n';
    // Implement the parsing logic here
    parseDirectives(block);
       
}
void LocationConfig::parseDirectives(const std::string &declarations)
{
    // Placeholder for actual directives parsing logic
    std::cout << "Parsing location directives:\n" << declarations << '\n';
    // Implement the parsing logic here
    std::istringstream stream(declarations);
    std::string line;
    while (std::getline(stream, line))
    {
        std::string directive;
        std::istringstream ss{trim(line)};
        ss >> directive;
        if (!directive.empty())
        {
            std::cout << "Directive: " << directive << '\n';
            // Implement the parsing logic here
            std::string value;
            ss >> value;
            if (directive == "autoindex")
            {
                autoIndex = (value == "on");
                std::cout << "Set autoindex to " << (autoIndex ? "on" : "off") << '\n';
            }
            else if (directive == "index")
            {
                indexFile = value;
                std::cout << "Set index file to " << indexFile << '\n';
            }
            else
            {
                directives[directive] = value;
            }
        }
    }
}
