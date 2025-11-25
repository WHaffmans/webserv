#include <webserv/config/ConfigManager.hpp>               // for ConfigManager
#include <webserv/config/validation/ConfigValidator.hpp>  // for ConfigValidator
#include <webserv/config/validation/ValidationResult.hpp> // for ValidationResult
#include <webserv/log/Log.hpp>                            // for Log
#include <webserv/server/Server.hpp>                      // for Server

#include <csignal>
#include <iostream> // for ios_base
#include <string>   // for allocator, basic_string, char_traits, operator+, string
#include <vector>   // for vector

void printHeader();

int main(int argc, char **argv)
{
    try
    {
        std::string configPath;

        // NOLINTBEGIN
        if ((argc == 3 || argc == 4) && std::string(argv[1]) == "-c")
        {
            configPath = argv[2];
        }
        else if (argc == 2)
        {
            configPath = argv[1];
        }
        else
        {
            std::cerr << "Usage: " + std::string(argv[0]) + " [-c] <config_file_path>"; // NOLINT
            return 1;
        }
        // NOLINTEND

        Log::setFileChannel("logs/webserv.log");
        Log::setStdoutChannel();
        // ::signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE globally
        printHeader();
        ConfigManager &configManager = ConfigManager::getInstance();
        configManager.init(configPath); // NOLINT

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

        ::signal(SIGINT, Server::signalHandler);
        server.run();
        return 0;
    }
    catch (const std::exception &e)
    {
        Log::error("error caught in main: " + std::string(e.what()));
        return 1;
    }
}

void printHeader()
{
    std::cout
        << " \033[2J\033[1;1H" // CLEAR SCREEN
        << "\n"
        << "\033[1;38;2;180;142;240m  ;;;;;;                       \033[0m\n"
        << "\033[1;38;2;173;130;233m  ;;;;;;;.                  +  \033[0m\n"
        << "\033[1;38;2;166;118;226m  ;;;;;;;;;               +++  \033[0m\n"
        << "\033[1;38;2;159;106;219m  ;;;;;;;;;;.           +++++  \033[0m\n"
        << "\033[1;38;2;152;94;212m  ;;;;;;+++++;        +++++++  \033[0m\n"
        << "\033[1;38;2;145;82;205m  +++++++++++++      ++++++++  +++++++++++xx xxx xxx     xxxxxxx       xxxx "
           "\033[0m\n"
        << "\033[1;38;2;138;70;198m  ++++++++++++++     ++++++++ ++++xxxxxxxxx  xxx xxxx    xxx xxxx     xxXX \033[0m\n"
        << "\033[1;38;2;131;58;191m  +++++++++++++++;   ++++++xx xxx;           xxx xxxxx   xxx  xXXX   XXXX \033[0m\n"
        << "\033[1;38;2;117;134;205m  ++++++++xxxxxxxxx  xxxxxxxx xxx    ;xxxxxx xxx xXXXXX. XXX   xXXX:XXXx \033[0m\n"
        << "\033[1;38;2;110;172;212m  xxxxxxx +xxxxxxxxx xxxxxxxx xxx    +xxxxxX XXX XXX XXX.XXX    xXXXXXX \033[0m\n"
        << "\033[1;38;2;103;210;219m  xxxxxxx   xxxxxxxxxxxxxxxxx xxx        XXX XXX XXX  XXXXXX     XXXXX \033[0m\n"
        << "\033[1;38;2;122;207;217m  xxxxxxx    xxxxxxxxxxxxxxXX XXXX       XXX XXX XXX   XXXXX     XXXX \033[0m\n"
        << "\033[1;38;2;125;196;218m  xxxxxxx     xxxxxXXXXXXXXXX  XXXXXXXXXXXXX XXX XXX    XXXX     XXXX \033[0m\n"
        << "\033[1;38;2;128;185;219m  xxxxxxx      xXXXXXXXXXXXXX    XXXXXXXXXX. XXX XXX     XXX     XXXX \033[0m\n"
        << "\033[1;38;2;131;174;220m  XXXXXXX       XXXXXXXXXXXXX \033[0m\n"
        << "\033[1;38;2;130;137;218m  XXXXX          XXXXXXXXXXXX \033[0m\n"
        << "\033[1;38;2;120;133;213m  XXX             XXXXXXXXXXX         A  W E B S E R V  P R O J E C T \033[0m\n"
        << "\033[1;38;2;110;129;208m  X                XXXXXXXXXX \033[0m\n"
        << "\033[1;38;2;100;125;203m                    XXXXXXXXX \033[0m\n"
        << "\033[1;38;2;90;121;198m                     XXXXXXXX \033[0m\n\n"
        << "\033[1;38;2;80;117;193m+===========================================================================+\033["
           "0m\n\n";
}
