#include "webserv/config/ConfigManager.hpp"
#include <webserv/config/AConfig.hpp>
#include <webserv/handler/ErrorHandler.hpp>
#include <webserv/http/HttpConstants.hpp> // for StatusCode
#include <webserv/log/Log.hpp>

#include <fstream>     // for basic_ifstream, basic_filebuf, basic_ostream::operator<<, ifstream, stringstream
#include <sstream>     // for basic_stringstream
#include <string>      // for basic_string, to_string, string
#include <string_view> // for string_view

std::string ErrorHandler::getErrorResponse(int statusCode, AConfig *config)
{
    std::string body = generateErrorPage(statusCode, config);
    Log::debug("Generated error page : " + generateErrorHeader(statusCode, body) + body);
    return generateErrorHeader(statusCode, body) + body;
}

std::string ErrorHandler::generateErrorHeader(int statusCode, const std::string &body)
{
    std::string response = "HTTP/1.1 ";
    response += std::to_string(statusCode) + " ";
    response += std::string(getStatusMessage(statusCode)) + std::string(Http::Protocol::CRLF);
    response += "Content-Type: text/html" + std::string(Http::Protocol::CRLF);
    response += "Content-Length: " + std::to_string(body.size()) + std::string(Http::Protocol::DOUBLE_CRLF); // End of headers
    return response;
}

std::string ErrorHandler::generateErrorPage(int statusCode, AConfig *config)
{

    if (config != nullptr)
    {
        config = ConfigManager::getInstance().getGlobalConfig();
        Log::info("Checking for custom error page for status code: " + std::to_string(statusCode)); 
        std::string customPage = config->getErrorPage(statusCode);
        if (!customPage.empty())
        {
            return getErrorPageFile(customPage);
        }
        Log::warning("No custom error page foundin config for status code: " + std::to_string(statusCode));
    }
    return generateDefaultErrorPage(statusCode);
}

std::string ErrorHandler::generateDefaultErrorPage(int statusCode)
{
    Log::info("Generating default error page for status code: " + std::to_string(statusCode));

    std::string_view statusMessage = getStatusMessage(statusCode);
    std::string html = "<html><head><title>" + std::to_string(statusCode) + " " + std::string(statusMessage) +
                       "</title></head><body><h1>" + std::to_string(statusCode) + " " + std::string(statusMessage) +
                       "</h1><hr><p>webserv</p></body></html>";
    return generateErrorHeader(statusCode, html) + html;
}

std::string_view ErrorHandler::getStatusMessage(int statusCode)
{
    for (const auto info : Http::statusCodeInfos)
    {
        if (info.code == statusCode)
        {
            return info.reason;
        }
    }
    return "Unknown Status";
}

std::string ErrorHandler::getErrorPageFile(const std::string &path)
{
    Log::info("Loading custom error page from: " + path);
    std::ifstream file(path.c_str());
    if (!file.is_open())
    {
        Log::error("Could not open custom error page: " + path);
        return generateErrorPage(Http::StatusCode::INTERNAL_SERVER_ERROR);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}