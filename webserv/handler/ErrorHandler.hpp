#pragma once

#include "webserv/http/HttpResponse.hpp"

#include <webserv/config/AConfig.hpp>
#include <memory>

class ErrorHandler
{
  public:
    static std::unique_ptr<HttpResponse> getErrorResponse(int statusCode, const AConfig *config = nullptr);

  private:
    static std::string generateErrorPage(int statusCode, const AConfig *config = nullptr);
    static std::string generateDefaultErrorPage(int statusCode);
    static std::string getErrorPageFile(const std::string &path);
};