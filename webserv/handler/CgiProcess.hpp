#pragma once

#include "webserv/handler/CgiHandler.hpp"
#include "webserv/http/HttpRequest.hpp"

class CgiProcess
{
  public:
    CgiProcess(const HttpRequest &request, CgiHandler &handler);

    CgiProcess(const CgiProcess &other) = delete;
    CgiProcess(CgiProcess &&other) noexcept = delete;

    CgiProcess &operator=(const CgiProcess &other) = delete;
    CgiProcess &operator=(CgiProcess &&other) noexcept = delete;

    ~CgiProcess() = default;

  private:
    const HttpRequest &request_;
    CgiHandler &handler_;

    int _pid;
    // int _cgiFd;

    void spawn();
};