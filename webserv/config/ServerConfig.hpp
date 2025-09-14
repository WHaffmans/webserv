#pragma once

#include <map>
#include <string>
#include <vector>

#include <webserv/config/LocationConfig.hpp>

class ServerConfig
{
  public:
    ServerConfig(const std::string &serverBlock);

    [[nodiscard]] LocationConfig getLocation(const std::string &path) const;

  private:
    std::string host;
    int port;
    std::string root;
    std::string cgi_pass;
    std::string cgi_ext;
    std::map<int, std::string> error_page;
    std::vector<std::string> index_files;
    std::map<std::string, LocationConfig> locations;

    void parseServerBlock(const std::string &block);
    void parseDirectives(const std::string &declarations);
};
