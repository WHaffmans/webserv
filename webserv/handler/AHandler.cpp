#include <webserv/handler/AHandler.hpp>

#include <webserv/http/HttpRequest.hpp>
#include <webserv/http/HttpResponse.hpp>

AHandler::AHandler(const HttpRequest &request, HttpResponse &response) : request_(request), response_(response) {}

