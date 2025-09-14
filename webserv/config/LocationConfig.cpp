#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/utils.hpp>

#include <iostream>
#include <sstream>
#include <string>

LocationConfig::LocationConfig(const std::string &locationBlock) : autoIndex(false)
{
    parseLocationBlock(locationBlock);
}

void LocationConfig::parseLocationBlock(const std::string &block)
{
    parseDirectives(block);
}
void LocationConfig::parseDirectives(const std::string &declarations)
{
    std::istringstream stream(declarations);
    std::string line;
    while (std::getline(stream, line))
    {
        std::string directive;
        std::istringstream lineStream{trim(line)};
        lineStream >> directive;
        if (!directive.empty())
        {
            std::string value;
            lineStream >> value;
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
