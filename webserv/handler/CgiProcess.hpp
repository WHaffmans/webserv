#pragma once

#include <webserv/handler/CgiHandler.hpp>
#include <webserv/http/HttpRequest.hpp> // for HttpRequest

class CgiHandler;

class CgiProcess
{
  public:
    CgiProcess(const HttpRequest &request, CgiHandler &handler);

    CgiProcess(const CgiProcess &other) = delete;
    CgiProcess(CgiProcess &&other) noexcept = delete;

    CgiProcess &operator=(const CgiProcess &other) = delete;
    CgiProcess &operator=(CgiProcess &&other) noexcept = delete;

    ~CgiProcess() = default;
    void kill() const noexcept;
    void wait() noexcept;

    [[nodiscard]] int getExitCode() const noexcept;

  private:
    const HttpRequest &request_;
    CgiHandler &handler_;

    int pid_;
    int status_;
    // int _cgiFd;

    void spawn();
};