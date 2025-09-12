#include <system_error>
#include <webserv/config/ConfigManager.hpp>

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file_path>\n";
        return 1;
    }
    ConfigManager::getInstance().init(argv[1]);

    return 0;
}