#include <webserv/config/ConfigManager.hpp> // for ConfigManager
#include <webserv/log/Log.hpp>              // for Log, LOCATION
#include <webserv/server/Server.hpp>        // for Server

#include <iostream> // for basic_ostream, operator<<, cerr, ios_base
#include <map>      // for map
#include <string>   // for basic_string, char_traits, allocator, operator+, operator<=>
#include <utility>  // for pair

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file_path>\n"; // NOLINT
        return 1;
    }
    Log::setFileChannel("webserv.log", std::ios_base::app, Log::Level::Trace);
    Log::setStdoutChannel(Log::Level::Trace);
    Log::info("\n======================\nStarting webserv...\n======================\n");
    Log::warning("Testing context: " + LOCATION, {{"key1", "value1"}, {"key2", "value2"}});
    ConfigManager::getInstance().init(argv[1]); // NOLINT
    Server server(ConfigManager::getInstance());

    server.start();
    return 0;
}