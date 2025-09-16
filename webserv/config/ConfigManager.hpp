#pragma once

#include <string>
#include <vector>

#include <webserv/config/ServerConfig.hpp>

class ConfigManager
{
  public:
    ConfigManager &operator=(const ConfigManager &other) = delete;
    ConfigManager &&operator=(const ConfigManager &&other) = delete;
    ConfigManager(const ConfigManager &other) = delete;
    ConfigManager(const ConfigManager &&other) = delete;

    void init(const std::string &filePath);
    static ConfigManager &getInstance();
    const std::vector<ServerConfig> &getServerConfigs() const { return serverConfigs; }

  private:
    bool _initialized;
    ConfigManager();
    ~ConfigManager();
    std::vector<ServerConfig> serverConfigs;

    void parseConfigFile(const std::string &filePath);
    // void parseGlobalDeclarations(const std::string &declarations);
};