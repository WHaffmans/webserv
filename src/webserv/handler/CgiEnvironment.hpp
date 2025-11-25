#pragma once

#include <webserv/handler/URI.hpp>
#include <webserv/http/HttpRequest.hpp> // for HttpRequest
#include <webserv/log/Log.hpp>
#include <webserv/utils/FileUtils.hpp>

#include <map>    // for map
#include <string> // for basic_string, string

class URI;
class HttpHeaders;

class CgiEnvironment
{
  public:
    CgiEnvironment(const URI &uri, const HttpRequest &request);

    std::string get(const std::string &key) const;
    [[nodiscard]] char **toEnvp() const;

  private:
    void setXHeaders(const HttpHeaders &headers);
    void addHttpHeaderToEnv(const std::string &headerName, const HttpHeaders &headers, const char *separator = "; ");
    std::map<std::string, std::string> env_;
};
