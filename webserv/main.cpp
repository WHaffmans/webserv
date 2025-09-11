#include <webserv/ConfigManager/ConfigManager.hpp>

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv)
{
    std::vector<std::string> args;
    args.reserve(static_cast<size_t>(argc));
    for (int i = 0; i < argc; ++i)
    {
        args.emplace_back(argv[i]);
    }

    if (args.size() < 2)
    {
        std::cerr << "Usage: " << args[0] << " <config_file_path>\n";
        return 1;
    }
    ConfigManager::getInstance().init(args[1]);
    return 0;
}