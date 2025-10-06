#include <webserv/config/ConfigManager.hpp> // for ConfigManager
#include <webserv/log/Log.hpp>              // for Log, LOCATION
#include <webserv/server/Server.hpp>        // for Server
#include <webserv/config/config_validator/ConfigValidator.hpp> // for ConfigValidator

#include <iostream> // for basic_ostream, operator<<, cerr, ios_base
#include <string>   // for basic_string, char_traits, allocator, operator+, operator<=>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        Log::error("Usage: " + std::string(argv[0]) + " <config_file_path>"); // NOLINT
        return 1;
    }

    Log::setFileChannel("webserv.log", std::ios_base::trunc, Log::Level::Info);
    Log::setStdoutChannel(Log::Level::Info);

    Log::info("\n======================\nStarting webserv...\n======================\n");
    ConfigManager &configManager = ConfigManager::getInstance();
    configManager.init(argv[1]); // NOLINT

    ConfigValidator validator{configManager.getGlobalConfig()};
    if (validator.hasErrors())
    {
        Log::error("Configuration validation failed with the following errors:");
        for (const auto &error : validator.getErrors())
        {
            Log::error(" - " + error.getMessage());
        }
        return 1;
    }

    Log::debug("ConfigManager initialized successfully.");
    Server server(configManager);

    server.start();
    return 0;
}