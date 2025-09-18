#include "webserv/log/Log.hpp"
#include <webserv/config/ConfigManager.hpp>
#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/server/Server.hpp>

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
    Server server(ConfigManager::getInstance());
    LOG_INFO("HALLO WERELD!");
    Log::info("test log message: server starting...", {{"port", "8080"}, {"mode", "production"}});
    server.start();

    return 0;
}