#include <webserv/config/ConfigManager.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/config/utils.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

ConfigManager::ConfigManager() : _initialized(false)
{
}

ConfigManager::~ConfigManager()
{
}

ConfigManager &ConfigManager::getInstance()
{
    static ConfigManager instance;
    std::cout << "ConfigManager instance" << '\n';
    return instance;
}

void ConfigManager::init(const std::string &filePath)
{
    if (_initialized)
    {
        throw std::runtime_error("ConfigManager is already initialized.");
    }
    std::cout << "Initializing ConfigManager with file: " << filePath << '\n';
    parseConfigFile(filePath);
    _initialized = true;
}

void removeEmptyLines(std::string &str)
{
    std::istringstream stream(str);
    std::string line;
    std::string result;

    while (std::getline(stream, line))
    {
        if (!trim(line).empty())
        {
            result += trimSemi(trim(line)) + '\n';
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
    std::cout << "Parsing configuration file: " << filePath << '\n';
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
        size_t closeBrace = findCorrespondingClosingBrace(content, bracePos);
        if (closeBrace == std::string::npos)
        {
            throw std::runtime_error("Malformed block in config file.");
        }
        // Optionally parse the server block here
        std::string serverBlock = content.substr(bracePos + 1, closeBrace - bracePos - 1);
        serverConfigs.emplace_back(serverBlock);
        pos = closeBrace + 1;
    }

    // parseGlobalDeclarations(globalDeclarations); // Implement this function to handle global config
    std::cout << "Global Declarations:\n" << globalDeclarations << '\n';
    file.close();
}

// void ConfigManager::parseGlobalDeclarations(const std::string &declarations)
// {
//     // Placeholder for actual global declarations parsing logic
//     std::cout << "Parsing global declarations:\n" << declarations << '\n';
//     // Implement the parsing logic here
// }
