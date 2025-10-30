#include <webserv/handler/CgiEnvironment.hpp> // for CgiEnvironment
#include <webserv/handler/CgiHandler.hpp>     // for CgiHandler
#include <webserv/handler/CgiProcess.hpp>
#include <webserv/handler/URI.hpp>      // for URI
#include <webserv/http/HttpRequest.hpp> // for HttpRequest
#include <webserv/log/Log.hpp>          // for Log
#include <webserv/socket/ASocket.hpp>   // for ASocket
#include <webserv/socket/CgiSocket.hpp> // for CgiSocket

#include <csignal>   // for kill, SIGKILL
#include <cstdlib>   // for exit
#include <memory>    // for allocator, make_unique, unique_ptr
#include <stdexcept> // for runtime_error
#include <string>    // for basic_string, operator+, to_string, char_traits, string
#include <utility>   // for move

#include <fcntl.h>    // for O_CLOEXEC, O_NONBLOCK
#include <sys/wait.h> // for waitpid, WNOHANG
#include <unistd.h>   // for close, dup2, pipe2, execve, fork, STDERR_FILENO, STDIN_FILENO, STDOUT_FILENO

CgiProcess::CgiProcess(const HttpRequest &request, CgiHandler &handler)
    : request_(request), handler_(handler), pid_(-1), status_(-1)
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

    // pipes
    // TODO pipe can handle flags O_CLOEXEC | O_NONBLOCK as open we should use this everywhere, then we dont need to set
    // non blocking and the fd will be closed when exec is run
    // NOLINTBEGIN
    int pipeStdin[2];
    int pipeStdout[2];
    int pipeStderr[2];

    if (pipe2(pipeStdin, O_CLOEXEC | O_NONBLOCK) == -1 || pipe2(pipeStdout, O_CLOEXEC | O_NONBLOCK) == -1
        || pipe2(pipeStderr, O_CLOEXEC | O_NONBLOCK) == -1)
    {
        throw std::runtime_error("Failed to create pipes");
    }
    // NOLINTEND
    CgiEnvironment cgiEnv(uri, request_);
    pid_ = fork();
    if (pid_ < 0)
    {
        close(pipeStdin[0]);
        close(pipeStdin[1]);
        close(pipeStdout[0]);
        close(pipeStdout[1]);
        close(pipeStderr[0]);
        close(pipeStderr[1]);
        throw std::runtime_error("Failed to fork");
    }
    if (pid_ == 0)
    {
        dup2(pipeStdin[0], STDIN_FILENO);
        dup2(pipeStdout[1], STDOUT_FILENO);
        dup2(pipeStderr[1], STDERR_FILENO);

        // close(pipeStdin[0]);
        // close(pipeStdin[1]);
        // close(pipeStdout[0]);
        // close(pipeStdout[1]);
        // close(pipeStderr[0]);
        // close(pipeStderr[1]);

        // Log::debug("Executing CGI: " + cgiPath);
        // std::cerr << "Executing CGI: " << cgiPath << std::endl;
        Log::clearChannels();

        // Prepare arguments
        std::string fullPath = uri.getFullPath();

        char *args[] = {const_cast<char *>(cgiPath.c_str()), const_cast<char *>(fullPath.c_str()), nullptr};
        // Log::debug("With args:", {args[0], args[1]});

        // TODO: Close all FDs
        execve(const_cast<char *>(cgiPath.c_str()), args, cgiEnv.toEnvp());
        exit(1);
    }
    else
    {
        // Parent process
        auto cgiStdIn = std::make_unique<CgiSocket>(pipeStdin[1], ASocket::IoState::WRITE);
        auto cgiStdOut = std::make_unique<CgiSocket>(pipeStdout[0], ASocket::IoState::READ);
        auto cgiStdErr = std::make_unique<CgiSocket>(pipeStderr[0], ASocket::IoState::READ);

        close(pipeStdin[0]);
        close(pipeStdout[1]);
        close(pipeStderr[1]);

        Log::debug("CGI process forked with PID: " + std::to_string(pid_));

        // request_.getClient().setCgiSockets(std::move(cgiStdIn), std::move(cgiStdOut)); // move the sockets to the
        // client
        handler_.setCgiSockets(std::move(cgiStdIn), std::move(cgiStdOut), std::move(cgiStdErr));

        handler_.setPid(pid_);
    }
}

void CgiProcess::kill() const noexcept
{
    if (pid_ > 0)
    {
        ::kill(pid_, SIGKILL);
        Log::debug("Killed CGI process with PID: " + std::to_string(pid_));
    }
}

void CgiProcess::wait() noexcept
{
    if (pid_ > 0)
    {
        int status;
        int waitResult = ::waitpid(pid_, &status, WNOHANG);
        if (waitResult == -1)
        {
            Log::error("Error while waiting for CGI process with PID: " + std::to_string(pid_));
            return;
        }
        if (waitResult == 0)
        {
            // Still running
            return;
        }

        Log::debug("CGI process with PID " + std::to_string(pid_) + " has terminated with status "
                   + std::to_string(WEXITSTATUS(status)));
        status_ = WEXITSTATUS(status);
        pid_ = -1;
    }
}

int CgiProcess::getExitCode() const noexcept
{
    return status_;
}