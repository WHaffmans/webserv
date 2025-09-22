#include <webserv/config/ConfigManager.hpp>
#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/log/Log.hpp>
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
    Log::setFileChannel("webserv.log", std::ios_base::app, Log::Level::Trace);
    Log::setStdoutChannel(Log::Level::Info);
    Log::info("\n======================\nStarting webserv...\n======================\n");
    Log::warning("Testing context", {{ "key1", "value1"}, {"key2", "value2"}});
    ConfigManager::getInstance().init(argv[1]); // NOLINT
    Server server(ConfigManager::getInstance());
    server.start();

    return 0;
}