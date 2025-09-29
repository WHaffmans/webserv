#include <webserv/handler/ErrorHandler.hpp>

#include <webserv/http/HttpConstants.hpp> // for StatusCode
#include <webserv/config/AConfig.hpp>
#include <webserv/log/Log.hpp>



// std::string ErrorHandler::generateErrorPage(int statusCode, AConfig *config)
// {
    
//     std::string statusMessage = getStatusMessage(statusCode);
//     std::string html =
//         "<html><head><title>" + std::to_string(statusCode) + " " + statusMessage +
//         "</title></head><body><h1>" + std::to_string(statusCode) + " " + statusMessage +
//         "</h1><hr><p>webserv</p></body></html>";
//     return html;
// }

