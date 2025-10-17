#include "webserv/handler/CgiProcess.hpp"

#include "webserv/http/HttpRequest.hpp"
#include "webserv/socket/CgiSocket.hpp"

#include <webserv/handler/URI.hpp>

#include <cstdlib>
#include <memory>
#include <string>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>

CgiProcess::CgiProcess(const HttpRequest &request, CgiHandler &handler) : request_(request), handler_(handler), _pid(-1)
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

    // pipes

    int pipeStdin[2];
    int pipeStdout[2];

    if (pipe(pipeStdin) == -1 || pipe(pipeStdout) == -1)
    {
        throw std::runtime_error("Failed to create pipes");
    }

    _pid = fork();
    if (_pid < 0)
    {
        close(pipeStdin[0]);
        close(pipeStdin[1]);
        close(pipeStdout[0]);
        close(pipeStdout[1]);
        throw std::runtime_error("Failed to fork");
    }
    if (_pid == 0)
    {
        dup2(pipeStdin[0], STDIN_FILENO);
        dup2(pipeStdout[1], STDOUT_FILENO);

        close(pipeStdin[0]);
        close(pipeStdin[1]);
        close(pipeStdout[0]);
        close(pipeStdout[1]);

        // Log::debug("Executing CGI: " + cgiPath);

        // Prepare arguments
        std::string fullPath = uri.getFullPath();
        char *args[] = {const_cast<char *>(cgiPath.c_str()), const_cast<char *>(fullPath.c_str()), nullptr};
        // Log::debug("With args:", {args[0], args[1]});

        // TODO: Close all FDs
        execve(const_cast<char *>(cgiPath.c_str()), args, nullptr);
        exit(1);
    }
    else
    {
        // Parent process
        auto cgiStdIn = std::make_unique<CgiSocket>(pipeStdin[1]);
        auto cgiStdOut = std::make_unique<CgiSocket>(pipeStdout[0]);
        close(pipeStdin[0]);
        close(pipeStdout[1]);

        Log::debug("CGI process forked with PID: " + std::to_string(_pid));

        // request_.getClient().setCgiSockets(std::move(cgiStdIn), std::move(cgiStdOut)); // move the sockets to the
        // client
        handler_.setCgiSockets(std::move(cgiStdIn), std::move(cgiStdOut));
        handler_.setPid(_pid);
    }
}

void CgiProcess::kill() const noexcept
{
    if (_pid > 0)
    {
        ::kill(_pid, SIGKILL);
        Log::debug("Killed CGI process with PID: " + std::to_string(_pid));
    }
}

void CgiProcess::wait() noexcept
{
    if (_pid > 0)
    {
        int status;
        int waitResult =::waitpid(_pid, &status, WNOHANG);
        if (waitResult == -1)
        {
            Log::error("Error while waiting for CGI process with PID: " + std::to_string(_pid));
            return;
        }
        if (waitResult == 0)
        {
            // Still running
            return;
        }

        Log::debug("CGI process with PID " + std::to_string(_pid) + " has terminated");
        _pid = -1;
    }
}