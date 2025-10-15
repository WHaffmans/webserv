#pragma once

#include <webserv/config/AConfig.hpp>
#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/http/HttpRequest.hpp> // for HttpRequest
#include <webserv/server/Server.hpp>

#include <map>    // for map
#include <string> // for string, basic_string

class LocationConfig;
class ServerConfig;
class AConfig;

class URI
{
  public:
    URI(const HttpRequest &request, const ServerConfig &serverConfig);

    [[nodiscard]] std::map<std::string, std::string> getCGIEnvironment() const;

    [[nodiscard]] bool isFile() const noexcept;
    [[nodiscard]] bool isDirectory() const noexcept;
    [[nodiscard]] bool isValid() const noexcept;
    [[nodiscard]] bool isCgi() const;

    [[nodiscard]] std::string getExtension() const noexcept;
    [[nodiscard]] std::string getCgiPath() const;
    [[nodiscard]] const AConfig *getConfig() const noexcept;
    [[nodiscard]] const std::string &getBaseName() const noexcept;
    [[nodiscard]] const std::string &getFullPath() const noexcept;
    [[nodiscard]] const std::string &getDir() const noexcept;
    [[nodiscard]] const std::string &getPathInfo() const noexcept;
    [[nodiscard]] const std::string &getQuery() const noexcept;
    [[nodiscard]] const std::string &getFragment() const noexcept;
    [[nodiscard]] const std::string &getAuthority() const noexcept;

  private:
    void parseUri(const std::string &uri);
    void parseFullpath();

    std::string uriTrimmed_;
    const AConfig *config_;
    std::string fullPath_; // dir_ + baseName_ + pathInfo_
    std::string baseName_;
    std::string dir_;
    std::string pathInfo_;
    std::string query_;
    std::string fragment_;
    std::string authority_;

    static const AConfig *matchConfig(const std::string &uri, const ServerConfig &serverConfig);
};