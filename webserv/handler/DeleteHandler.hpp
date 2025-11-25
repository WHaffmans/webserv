#pragma once

#include <webserv/handler/AHandler.hpp> // for AHandler
#include <webserv/http/HttpRequest.hpp> // for HttpRequest

#include <string> // for string

class HttpResponse;

class DeleteHandler : public AHandler
{
  public:
    DeleteHandler(const HttpRequest &request, HttpResponse &response);

    void handle() override;
    void handleTimeout() override;

  private:
    void deleteFile(const std::string &path);
    void deleteDirectory(const std::string &path);
};