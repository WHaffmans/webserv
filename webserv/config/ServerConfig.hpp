#pragma once

#include <webserv/config/LocationConfig.hpp>

#include <map>
#include <string>
#include <vector>

class ServerConfig
{
  public:
    ServerConfig(const std::string &serverBlock);

    [[nodiscard]] const std::string &getHost() const { return host; }
    [[nodiscard]] int getPort() const { return port; }
    [[nodiscard]] const std::string &getRoot() const { return root; }
    [[nodiscard]] const std::string &getCgiPass() const { return cgi_pass; }
    [[nodiscard]] const std::string &getCgiExt() const { return cgi_ext; }
    [[nodiscard]] const std::map<int, std::string> &getErrorPages() const { return error_page; }
    [[nodiscard]] const std::vector<std::string> &getIndexFiles() const { return index_files; }
    [[nodiscard]] const LocationConfig &getLocation(const std::string &path) const;
    [[nodiscard]] std::vector<std::string> getLocationPaths() const;

    void setServerFD(int fd) { server_fd = fd; }
    [[nodiscard]] int getServerFD() const { return server_fd; }

  private:
    std::string host;
    int port;
    int server_fd;
    std::string root;
    std::string cgi_pass;
    std::string cgi_ext;
    std::map<int, std::string> error_page;
    std::vector<std::string> index_files;
    std::map<std::string, LocationConfig> locations;

    void parseServerBlock(const std::string &block);
    void parseDirectives(const std::string &declarations);
};
