#include <webserv/handler/DeleteHandler.hpp>

#include <webserv/handler/ErrorHandler.hpp> // for ErrorHandler
#include <webserv/handler/URI.hpp>          // for URI
#include <webserv/http/HttpConstants.hpp>   // for FORBIDDEN, NO_CONTENT, BAD_REQUEST, GATEWAY_TIMEOUT, NOT_FOUND
#include <webserv/http/HttpResponse.hpp>    // for HttpResponse
#include <webserv/log/Log.hpp>              // for Log, LOCATION
#include <webserv/utils/FileUtils.hpp>      // for isDirectory, isFile, isValidPath

#include <stdio.h>  // for remove
#include <unistd.h> // for rmdir

DeleteHandler::DeleteHandler(const HttpRequest &request, HttpResponse &response) : AHandler(request, response) {}

void DeleteHandler::handle()
{
    Log::trace(LOCATION);
    // delete file:
    std::string fullPath = request_.getUri().getFullPath();
    if (fullPath.empty() || !FileUtils::isValidPath(fullPath))
    {
        Log::warning("DeleteHandler: Invalid path for DELETE: " + fullPath);
        ErrorHandler::createErrorResponse(Http::StatusCode::BAD_REQUEST, response_, request_.getUri().getConfig());
        return;
    }
    if (FileUtils::isFile(fullPath))
    {
        deleteFile(fullPath);
    }
    else if (FileUtils::isDirectory(fullPath))
    {
        deleteDirectory(fullPath);
    }
    else
    {
        Log::warning("DeleteHandler: Path not found for DELETE: " + fullPath);
        ErrorHandler::createErrorResponse(Http::StatusCode::NOT_FOUND, response_, request_.getUri().getConfig());
    }
}

void DeleteHandler::deleteFile(const std::string &path)
{
    Log::trace(LOCATION);
    if (remove(path.c_str()) != 0)
    {
        Log::error("DeleteHandler: Failed to delete file: " + path);
        ErrorHandler::createErrorResponse(Http::StatusCode::FORBIDDEN, response_, request_.getUri().getConfig());
    }
    else
    {
        Log::info("DeleteHandler: Successfully deleted file: " + path);
        response_.setStatus(Http::StatusCode::NO_CONTENT);
        response_.setComplete();
    }
}

void DeleteHandler::deleteDirectory(const std::string &path)
{
    Log::trace(LOCATION);

    // Only allow deletion of empty directories
    if (rmdir(path.c_str()) != 0)
    {
        Log::error("DeleteHandler: Failed to delete directory");
        ErrorHandler::createErrorResponse(Http::StatusCode::FORBIDDEN, response_, request_.getUri().getConfig());
    }
    else
    {
        Log::info("DeleteHandler: Successfully deleted empty directory: " + path);
        response_.setStatus(Http::StatusCode::NO_CONTENT);
        response_.setComplete();
    }
}

void DeleteHandler::handleTimeout()
{
    Log::debug("DeleteHandler: Request timed out");
    ErrorHandler::createErrorResponse(Http::StatusCode::GATEWAY_TIMEOUT, response_, request_.getUri().getConfig());
}