#pragma once

#include <webserv/handler/AHandler.hpp> // for AHandler

class RedirectHandler : public AHandler
{
  public:
    RedirectHandler(const HttpRequest &request, HttpResponse &response);

    void handle() override;

  protected:
    void handleTimeout() override;

  private:
};
