#pragma once

#include <webserv/config/AConfig.hpp>

class ErrorHandler
{
  public:
    static std::string getErrorResponse(int statusCode, AConfig *config = nullptr);
    static std::string generateErrorPage(int statusCode, AConfig *config = nullptr);
    static std::string generateErrorHeader(int statusCode, const std::string &body);
    static std::string generateDefaultErrorPage(int statusCode);
    static std::string_view getStatusMessage(int statusCode);
    // static bool isValidStatusCode(int statusCode);
    static std::string getErrorPageFile(const std::string &path);
};