#pragma once

#include <webserv/config/AConfig.hpp>
#include <webserv/http/HttpResponse.hpp> // for HttpResponse

#include <memory> // for unique_ptr
#include <string> // for string

#include <stdint.h> // for uint16_t
#include <sys/types.h>

class AConfig;

class ErrorHandler
{
  public:
    static std::unique_ptr<HttpResponse> getErrorResponse(uint16_t statusCode, const AConfig *config = nullptr);

  private:
    static std::string generateErrorPage(uint16_t statusCode, const AConfig *config = nullptr);
    static std::string generateDefaultErrorPage(uint16_t statusCode);
    static std::string getErrorPageFile(const std::string &path);
};