#include "webserv/config/GlobalConfig.hpp" // for GlobalConfig

#include <webserv/config/ConfigManager.hpp>
#include <webserv/config/utils.hpp> // for removeComments
#include <webserv/log/Log.hpp>      // for Log

#include <fstream>   // for basic_ifstream, basic_filebuf, basic_ostream::operator<<, ifstream, stringstream
#include <sstream>   // for basic_stringstream
#include <stdexcept> // for runtime_error
#include <string>    // for char_traits, operator+, basic_string, string

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
    utils::removeComments(content);
    globalConfig_ = std::make_unique<GlobalConfig>(content);

    // Implement this function to handle global config
    file.close();
}

std::vector<ServerConfig *> ConfigManager::getServerConfigs() const
{
    if (!initialized_)
    {
        throw std::runtime_error("ConfigManager is not initialized.");
    }
    return globalConfig_->getServerConfigs();
}