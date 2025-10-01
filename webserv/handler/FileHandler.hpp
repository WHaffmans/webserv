#pragma once

#include "webserv/config/LocationConfig.hpp"
#include "webserv/handler/URIParser.hpp"
#include "webserv/http/HttpResponse.hpp"

#include <cstdio>
#include <memory>

class FileHandler
{
  public:
    FileHandler(const LocationConfig *location, const URIParser &uriParser);

    [[nodiscard]] std::unique_ptr<HttpResponse> getResponse() const;
    static std::vector<char> readBinaryFile(const std ::string &filepath);
    static std::string readFileAsString(const std::string &filepath);

  private:
    // std::unique_ptr<HttpResponse> response_;
    const LocationConfig *location_;
    const URIParser &uriParser_;
};