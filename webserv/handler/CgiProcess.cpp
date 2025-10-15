#include "webserv/handler/CgiProcess.hpp"

#include "webserv/http/HttpRequest.hpp"
#include "webserv/socket/CgiSocket.hpp"

#include <webserv/handler/URI.hpp>

#include <memory>
#include <string>

#include <sys/socket.h>
#include <unistd.h>

CgiProcess::CgiProcess(const HttpRequest &request) : request_(request), _pid(-1), _cgiFd(-1)
{
    if (!request_.getUri().isCgi())
    {
        throw std::runtime_error("URI is not a CGI");
    }

    spawn();
}

void CgiProcess::spawn()
{
    const URI &uri = request_.getUri();
    auto cgiPath = uri.getCgiPath();
    auto environment = uri.getCGIEnvironment();

    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0)
    {
        throw std::runtime_error("Failed to create socket pair");
    }

    _cgiFd = sv[1];
    _pid = fork();
    if (_pid < 0)
    {
        close(sv[0]);
        close(sv[1]);
        throw std::runtime_error("Failed to fork");
    }
    if (_pid == 0)
    {
        dup2(sv[1], STDIN_FILENO);
        dup2(sv[1], STDOUT_FILENO);

        close(sv[0]);
        close(sv[1]);

        // Prepare arguments
        std::string fullPath = uri.getFullPath();
        char *args[] = {const_cast<char *>(cgiPath.c_str()), const_cast<char *>(fullPath.c_str())};
        execve(const_cast<char *>(cgiPath.c_str()), args, nullptr);
    }
    else
    {
        // Parent process
        auto cgiSocket = std::make_unique<CgiSocket>(sv[0]); // CgiSocket wraps sv[0]
        close(sv[1]);

        // cgiSocket->write(request_.getBody().data(), request_.getBody().size());
        request_.getClient().setCgiSocket(std::move(cgiSocket)); // move the socket to the client
        // _cgiFd can be used to communicate with the CGI process
    }
}