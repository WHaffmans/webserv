#pragma once

#include <webserv/handler/AHandler.hpp> // for AHandler

class UploadHandler : public AHandler
{
  public:
    UploadHandler(const HttpRequest &request, HttpResponse &response);

    void handle() override;
    void handleTimeout() override;

  private:
    void uploadFile(const std::string &path, const std::string &data);
};