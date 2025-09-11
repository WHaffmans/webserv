#include <iostream>
#include <webserv/ConfigManager/ConfigManager.hpp>

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
    // parseConfigFile(filePath);
    _initialized = true;
}