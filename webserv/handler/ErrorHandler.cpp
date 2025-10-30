#include <webserv/handler/ErrorHandler.hpp> // for ErrorHandler

#include <webserv/config/AConfig.hpp>       // for AConfig
#include <webserv/config/ConfigManager.hpp> // for ConfigManager
#include <webserv/config/GlobalConfig.hpp>  // for GlobalConfig
#include <webserv/http/HttpConstants.hpp>   // for getStatusCodeReason, INTERNAL_SERVER_ERROR
#include <webserv/http/HttpResponse.hpp>    // for HttpResponse
#include <webserv/log/Log.hpp>              // for Log, LOCATION
#include <webserv/utils/utils.hpp>          // for join

#include <fstream> // for basic_ifstream, basic_filebuf, basic_ostream::operator<<, ifstream, stringstream
#include <sstream> // for basic_stringstream
#include <string>  // for allocator, basic_string, char_traits, operator+, string, to_string

void ErrorHandler::createErrorResponse(uint16_t statusCode, HttpResponse &response, const AConfig *config)
{
    std::string statusMessage = Http::getStatusCodeReason(statusCode);
    Log::warning("Generating error response: " + std::to_string(statusCode) + " " + statusMessage);

    response.setStatus(statusCode);
    response.addHeader("Content-Type", "text/html");
    response.addHeader("Connection", "close");
    if (statusCode == Http::StatusCode::METHOD_NOT_ALLOWED && config != nullptr)
    {
        auto allowedMethods = config->get<std::vector<std::string>>("allowed_methods");
        if (allowedMethods.has_value())
            response.addHeader("Allow", utils::implode(allowedMethods.value(), ", "));
    }
    response.appendBody(generateErrorPage(statusCode, config));
    response.setComplete();
}

std::string ErrorHandler::generateErrorPage(uint16_t statusCode, const AConfig *config)
{
    Log::trace(LOCATION);
    if (config == nullptr)
    {
        config = ConfigManager::getInstance().getGlobalConfig();
        Log::debug("Using global config for error page generation");
    }
    if (config != nullptr)
    {
        Log::info("Checking for custom error page");
        std::string customPage = config->getErrorPage(statusCode);
        if (!customPage.empty())
        {
            return getErrorPageFile(customPage);
        }
        Log::warning("No custom error page found");
    }
    return generateDefaultErrorPage(statusCode);
}

std::string ErrorHandler::generateDefaultErrorPage(uint16_t statusCode)
{
    Log::info("Generating default error page");
    std::string statusMessage = Http::getStatusCodeReason(statusCode);
    std::string html = "<html><head><title>" + std::to_string(statusCode) + " " + statusMessage
                       + "</title></head><body><h1>" + std::to_string(statusCode) + " " + statusMessage
                       + "</h1><hr><p>webserv</p></body></html>";
    return html;
}

std::string ErrorHandler::getErrorPageFile(const std::string &path)
{
    Log::debug("Loading custom error page from: " + path);
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