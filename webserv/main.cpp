#include <webserv/config/ConfigManager.hpp>               // for ConfigManager
#include <webserv/config/validation/ConfigValidator.hpp>  // for ConfigValidator
#include <webserv/config/validation/ValidationResult.hpp> // for ValidationResult
#include <webserv/log/Log.hpp>                            // for Log
#include <webserv/server/Server.hpp>                      // for Server

#include <iostream> // for ios_base
#include <string>   // for allocator, basic_string, char_traits, operator+, string
#include <vector>   // for vector

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        Log::error("Usage: " + std::string(argv[0]) + " <config_file_path>"); // NOLINT
        return 1;
    }

    Log::setFileChannel("logs/webserv.log", std::ios_base::trunc);
    Log::setStdoutChannel();

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

    server.run();
    return 0;
}