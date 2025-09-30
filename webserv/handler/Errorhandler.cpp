#include "webserv/http/HttpResponse.hpp"

#include <webserv/config/AConfig.hpp>       // for AConfig
#include <webserv/config/ConfigManager.hpp> // for ConfigManager
#include <webserv/config/GlobalConfig.hpp>  // for GlobalConfig
#include <webserv/handler/ErrorHandler.hpp> // for ErrorHandler
#include <webserv/http/HttpConstants.hpp> // for StatusCodeInfo, CRLF, DOUBLE_CRLF, INTERNAL_SERVER_ERROR, statusCodeInfos
#include <webserv/log/Log.hpp>            // for Log

#include <fstream> // for basic_ifstream, basic_filebuf, basic_ostream::operator<<, ifstream, stringstream
#include <sstream> // for basic_stringstream
#include <string>  // for basic_string, operator+, allocator, char_traits, string, to_string

HttpResponse ErrorHandler::getErrorResponse(int statusCode, AConfig *config)
{
    HttpResponse response;

    std::string body = generateErrorPage(statusCode, config);
    response.appendBody(body);
    response.setStatus(statusCode);
    response.addHeader("Content-Type", "text/html");
    response.addHeader("Connection", "close");
    return response;
}

std::string ErrorHandler::generateErrorPage(int statusCode, AConfig *config)
{
    Log::trace(LOCATION);
    if (config == nullptr)
    {
        config = ConfigManager::getInstance().getGlobalConfig();
        Log::debug("Using global config for error page generation");
    }
    if (config != nullptr)
    {
        Log::info("Checking for custom error page for status code: " + std::to_string(statusCode));
        std::string customPage = config->getErrorPage(statusCode);
        if (!customPage.empty())
        {
            return getErrorPageFile(customPage);
        }
        Log::warning("No custom error page found in config for status code: " + std::to_string(statusCode));
    }
    return generateDefaultErrorPage(statusCode);
}

std::string ErrorHandler::generateDefaultErrorPage(int statusCode)
{
    Log::info("Generating default error page for status code: " + std::to_string(statusCode));
    std::string statusMessage = Http::getStatusCodeReason(statusCode);
    std::string html = "<html><head><title>" + std::to_string(statusCode) + " " + statusMessage +
                       "</title></head><body><h1>" + std::to_string(statusCode) + " " + statusMessage +
                       "</h1><hr><p>webserv</p></body></html>";
    return html;
}

std::string ErrorHandler::getErrorPageFile(const std::string &path)
{
    Log::info("Loading custom error page from: " + path);
    std::ifstream file(path.c_str());
    if (!file.is_open())
    {
        Log::error("Could not open custom error page: " + path);
        return generateDefaultErrorPage(Http::StatusCode::INTERNAL_SERVER_ERROR);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}