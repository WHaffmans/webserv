#pragma once

#include <webserv/handler/URI.hpp>
#include <webserv/http/HttpRequest.hpp> // for HttpRequest
#include <webserv/log/Log.hpp>
#include <webserv/utils/FileUtils.hpp>

#include <map>    // for map
#include <string> // for basic_string, string

class URI;

class CgiEnvironment
{
  public:
    CgiEnvironment(const URI &uri, const HttpRequest &request);

    [[nodiscard]] char **toEnvp() const;

  private:
    std::map<std::string, std::string> env_;
};
