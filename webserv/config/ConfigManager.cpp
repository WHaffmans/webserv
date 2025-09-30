#include <webserv/config/ConfigManager.hpp>

#include <webserv/config/GlobalConfig.hpp> // for GlobalConfig
#include <webserv/config/utils.hpp>        // for removeComments
#include <webserv/log/Log.hpp>             // for Log

#include <fstream>   // for basic_ifstream, basic_filebuf, basic_ostream::operator<<, ifstream, stringstream
#include <sstream>   // for basic_stringstream
#include <stdexcept> // for runtime_error
#include <string>    // for basic_string, char_traits, operator+, string, to_string, operator==, stoi

#include <stddef.h> // for size_t

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

ServerConfig *ConfigManager::getMatchingServerConfig(const std::string &host_port) const
{
    if (!initialized_)
    {
        throw std::runtime_error("ConfigManager is not initialized.");
    }
    // split host and port on the colon:
    size_t colonPos = host_port.find(':');
    std::string host = (colonPos != std::string::npos) ? host_port.substr(0, colonPos) : host_port;
    int port = (colonPos != std::string::npos) ? std::stoi(host_port.substr(colonPos + 1)) : 0;
    return getMatchingServerConfig(host, port);
}

ServerConfig *ConfigManager::getMatchingServerConfig(const std::string &host, int port) const
{
    if (!initialized_)
    {
        throw std::runtime_error("ConfigManager is not initialized.");
    }
    std::vector<ServerConfig *> serverConfigs = globalConfig_->getServerConfigs();
    for (ServerConfig *serverConfig : serverConfigs)
    {
        auto serverName = serverConfig->getDirectiveValue<std::string>("server_name", "");
        auto listenPorts = serverConfig->getDirectiveValue<int>("listen", 80);
        Log::debug("Checking server config: " + serverName + " on port " + std::to_string(listenPorts));
        if ((serverName == host) && (listenPorts == port))
        {
            Log::info("Found matching server config for host: " + host + " and port: " + std::to_string(port));
            return serverConfig;
        }
    }
    Log::warning("No matching server config found for host: " + host + " and port: " + std::to_string(port));
    return nullptr;
}

GlobalConfig *ConfigManager::getGlobalConfig() const
{
    if (!initialized_)
    {
        throw std::runtime_error("ConfigManager is not initialized.");
    }
    return globalConfig_.get();
}