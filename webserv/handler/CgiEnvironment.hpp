#pragma once

#include <map>
#include <string>
#include "webserv/handler/URI.hpp"
#include "webserv/http/HttpRequest.hpp"
#include "webserv/utils/FileUtils.hpp"
#include "webserv/log/Log.hpp"

class CgiEnvironment
{
public:
  CgiEnvironment(const URI &uri, const HttpRequest &request);

  [[nodiscard]] char **toEnvp() const;


private:
  std::map<std::string, std::string> env_;
  
};
