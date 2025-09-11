#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

struct LocationConfig
{
    std::string path;
    bool autoIndex;
    std::string indexFile;
    // Add other location-specific configurations as needed
};

struct ServerConfig
{
    std::string host;
    int port;
    std::string root;
    // Add other server-specific configurations as needed
    std::unique_ptr<std::map<std::string, LocationConfig>> locations;
};

class ConfigManager
{
  public:
    ConfigManager &operator=(const ConfigManager &other) = delete;
    ConfigManager &&operator=(const ConfigManager &&other) = delete;
    ConfigManager(const ConfigManager &other) = delete;
    ConfigManager(const ConfigManager &&other) = delete;

    void init(const std::string &filePath);
    static ConfigManager &getInstance();
    
    private:
    bool _initialized;
    ConfigManager();
    ~ConfigManager();
    std::vector<ServerConfig> serverConfigs;

    // void parseConfigFile(const std::string &filePath);
    // void parseServerBlock(const std::string &block);
    // void parseLocationBlock(const std::string &block, ServerConfig &serverConfig);
};