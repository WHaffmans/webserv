#pragma once

#include <webserv/config/ServerConfig.hpp>
#include <string>
#include <vector>

class ConfigManager
{
  public:
    ConfigManager &operator=(const ConfigManager &other) = delete;
    ConfigManager &&operator=(const ConfigManager &&other) = delete;
    ConfigManager(const ConfigManager &other) = delete;
    ConfigManager(const ConfigManager &&other) = delete;

    void init(const std::string &filePath);
    static ConfigManager &getInstance();

    [[nodiscard]] const std::vector<ServerConfig> &getServerConfigs() const { return serverConfigs_; }

  private:
    bool initialized_;
    ConfigManager();
    ~ConfigManager();
    std::vector<ServerConfig> serverConfigs_;

    void parseConfigFile(const std::string &filePath);
    // void parseGlobalDeclarations(const std::string &declarations);
};