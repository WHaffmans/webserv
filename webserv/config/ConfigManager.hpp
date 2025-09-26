#pragma once

#include <webserv/config/GlobalConfig.hpp>
#include <webserv/config/ServerConfig.hpp> // for ServerConfig

#include <memory> // for unique_ptr
#include <string> // for string
#include <vector> // for vector

class ADirective;

class ConfigManager
{
  public:
    ConfigManager &operator=(const ConfigManager &other) = delete;
    ConfigManager &&operator=(const ConfigManager &&other) = delete;
    ConfigManager(const ConfigManager &other) = delete;
    ConfigManager(const ConfigManager &&other) = delete;

    void init(const std::string &filePath);
    static ConfigManager &getInstance();

    [[nodiscard]] std::vector<ServerConfig *> getServerConfigs() const;

  private:
    bool initialized_;
    ConfigManager();
    ~ConfigManager();
    std::unique_ptr<GlobalConfig> globalConfig_;

    void parseConfigFile(const std::string &filePath);
};