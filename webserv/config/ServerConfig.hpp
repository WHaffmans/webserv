#pragma once

#include "webserv/config/directive/ADirective.hpp"
#include <webserv/config/LocationConfig.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

// TODO remove implementation details from header
class ServerConfig
{
  public:
    ServerConfig(const std::string &serverBlock);

    // [[nodiscard]] const std::string &getHost() const { return host_; }

    // [[nodiscard]] int getPort() const { return port_; }

    // [[nodiscard]] const std::string &getRoot() const { return root_; }

    // [[nodiscard]] const std::string &getCgiPass() const { return cgi_pass_; }

    // [[nodiscard]] const std::string &getCgiExt() const { return cgi_ext_; }

    // [[nodiscard]] const std::map<int, std::string> &getErrorPages() const { return error_page_; }

    // [[nodiscard]] const std::vector<std::string> &getIndexFiles() const { return index_files_; }

    // [[nodiscard]] const LocationConfig &getLocation(const std::string &path) const;
    // [[nodiscard]] std::vector<std::string> getLocationPaths() const;

    DirectiveValue operator[](const std::string &directive) const;

  private:
    std::string host_;
    int port_;
    std::string root_;
    std::string cgi_pass_;
    std::string cgi_ext_;
    std::map<int, std::string> error_page_;
    std::vector<std::string> index_files_;
    std::map<std::string, LocationConfig> locations_;
    std::vector<std::unique_ptr<ADirective>> directives_;

    void parseServerBlock(const std::string &block);
    void parseDirectives(const std::string &declarations);
};
