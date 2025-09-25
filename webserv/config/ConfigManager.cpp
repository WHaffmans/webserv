#include <webserv/config/ConfigManager.hpp>
#include <webserv/config/ServerConfig.hpp>               // for ServerConfig
#include <webserv/config/directive/DirectiveFactory.hpp> // for DirectiveFactory
#include <webserv/config/utils.hpp>                      // for trim, findCorrespondingClosingBrace, trimSemi
#include <webserv/log/Log.hpp>                           // for Log

#include <cstddef> // for size_t
#include <fstream> // for basic_ifstream, basic_istream, basic_filebuf, basic_ostream::operator<<, ifstream, istringstream, stringstream
#include <sstream>   // for basic_stringstream, basic_istringstream
#include <stdexcept> // for runtime_error
#include <string>

ConfigManager::ConfigManager() : initialized_(false) {}

ConfigManager::~ConfigManager() {}

ConfigManager &ConfigManager::getInstance()
{
    static ConfigManager instance;
    return instance;
}

void ConfigManager::init(const std::string &filePath)
{
    if (initialized_)
    {
        Log::warning("ConfigManager is already initialized.");
        throw std::runtime_error("ConfigManager is already initialized.");
    }
    Log::info("Initializing ConfigManager with file: " + filePath);
    parseConfigFile(filePath);
    initialized_ = true;
}

void removeEmptyLines(std::string &str)
{
    std::istringstream stream(str);
    std::string line;
    std::string result;

    while (std::getline(stream, line))
    {
        if (!utils::trim(line).empty())
        {
            result += utils::trimSemi(utils::trim(line)) + '\n';
        }
    }
    str = result;
}

void removeComments(std::string &str)
{
    size_t pos = 0;
    while ((pos = str.find('#', pos)) != std::string::npos)
    {
        size_t end = str.find('\n', pos);
        if (end == std::string::npos)
        {
            str.erase(pos);
        }
        else
        {
            str.erase(pos, end - pos);
        }
    }
    removeEmptyLines(str);
}

void ConfigManager::parseConfigFile(const std::string &filePath)
{
    // Placeholder for actual file parsing logic
    Log::info("Parsing configuration file: " + filePath);
    // Implement the parsing logic here

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open config file: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    removeComments(content);

    std::string globalDeclarations;
    Log::trace("Content before parsing servers:\n" + content);
    size_t pos = 0;
    while (true)
    {
        size_t serverPos = content.find("server", pos);
        size_t bracePos = content.find('{', serverPos);
        if (serverPos == std::string::npos || bracePos == std::string::npos)
        {
            // No more server blocks, remaining is global
            globalDeclarations += content.substr(pos);
            break;
        }
        // Add global declarations before this server block
        globalDeclarations += content.substr(pos, serverPos - pos);
        Log::trace(LOCATION, {{"pos", std::to_string(pos)},
                              {"serverPos", std::to_string(serverPos)},
                              {"globalDeclarations", globalDeclarations}});
        size_t closeBrace = utils::findCorrespondingClosingBrace(content, bracePos);
        if (closeBrace == std::string::npos)
        {
            throw std::runtime_error("Malformed block in config file.");
        }
        // Optionally parse the server block here
        std::string serverBlock = content.substr(bracePos + 1, closeBrace - bracePos - 1);
        serverConfigs_.emplace_back(serverBlock);
        pos = closeBrace + 1;
    }

    parseGlobalDeclarations(globalDeclarations); // Implement this function to handle global config
    file.close();
}

void ConfigManager::parseGlobalDeclarations(const std::string &declarations)
{
    Log::trace(LOCATION);
    std::stringstream ss(declarations);
    std::string line;
    while (ss.good())
    {
        std::getline(ss, line);
        line = utils::trim(line);
        if (line.empty())
        {
            continue;
        }
        Log::info("Global Declaration: " + line);
        auto directive = DirectiveFactory::createDirective(line);
        globalDirectives_.push_back(std::move(directive));
    }
}
