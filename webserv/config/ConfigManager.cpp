#include <webserv/config/ConfigManager.hpp>
#include <webserv/config/ServerConfig.hpp>               // for ServerConfig
#include <webserv/config/directive/DirectiveFactory.hpp> // for DirectiveFactory
#include <webserv/config/utils.hpp>                      // for trim, findCorrespondingClosingBrace, trimSemi
#include <webserv/log/Log.hpp>                           // for Log, LOCATION

#include <fstream> // for basic_ifstream, basic_istream, basic_filebuf, basic_ostream::operator<<, stringstream, ifstream, istringstream
#include <sstream>   // for basic_stringstream, basic_istringstream
#include <stdexcept> // for runtime_error
#include <string>    // for basic_string, char_traits, string, operator+, to_string, getline, operator<=>

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