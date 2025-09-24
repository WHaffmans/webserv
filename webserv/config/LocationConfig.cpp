#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/utils.hpp>
#include <webserv/log/Log.hpp>

#include <sstream>
#include <string>

LocationConfig::LocationConfig(const std::string &locationBlock) : autoIndex_(false)
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
                autoIndex_ = (value == "on");
                Log::info("Set autoindex to " + std::string(autoIndex_ ? "on" : "off"));
            }
            else if (directive == "index")
            {
                indexFile_ = value;
                Log::info("Set index file to " + indexFile_);
            }
            else
            {
                directives_[directive] = value;
            }
        }
    }
}
