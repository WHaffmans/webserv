#include <webserv/config/ConfigManager.hpp>
#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/ServerConfig.hpp>

#include <iostream>
#include <string>

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file_path>\n"; // NOLINT
        return 1;
    }
    ConfigManager::getInstance().init(argv[1]); // NOLINT

    const auto &serverConfigs = ConfigManager::getInstance().getServerConfigs();
    for (const auto &serverConfig : serverConfigs)
    {
        std::cout << "Server " << serverConfig.getHost() << " listening on port: " << serverConfig.getPort() << '\n';

        for (const auto &path : serverConfig.getLocationPaths())
        {
            std::cout << "  Location: " << path << '\n';
        }
    }

    return 0;
}