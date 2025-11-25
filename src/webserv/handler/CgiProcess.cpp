#include <webserv/handler/CgiProcess.hpp>

#include <webserv/handler/CgiEnvironment.hpp> // for CgiEnvironment
#include <webserv/handler/URI.hpp>      // for URI
#include <webserv/http/HttpRequest.hpp> // for HttpRequest
#include <webserv/log/Log.hpp>          // for Log
#include <webserv/socket/ASocket.hpp>   // for ASocket
#include <webserv/socket/CgiSocket.hpp> // for CgiSocket

#include <csignal>   // for kill, SIGKILL
#include <cstdlib>   // for WEXITSTATUS, exit
#include <memory>    // for allocator, make_unique, unique_ptr
#include <stdexcept> // for runtime_error
#include <string>    // for char_traits, basic_string, operator+, to_string, string
#include <utility>   // for move

#include <fcntl.h>    // for fcntl, O_NONBLOCK, F_GETFL, F_SETFL, O_CLOEXEC
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

CgiProcess::~CgiProcess()
{
    this->kill();
}

void CgiProcess::spawn()
{
    const URI &uri = request_.getUri();
    auto cgiPath = uri.getCgiPath();

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
        int flags = fcntl(pipeStdin[0], F_GETFL, 0);
        fcntl(pipeStdin[0], F_SETFL, flags & ~O_NONBLOCK);

        flags = fcntl(pipeStdout[1], F_GETFL, 0);
        fcntl(pipeStdout[1], F_SETFL, flags & ~O_NONBLOCK);

        flags = fcntl(pipeStderr[1], F_GETFL, 0);
        fcntl(pipeStderr[1], F_SETFL, flags & ~O_NONBLOCK);

        dup2(pipeStdin[0], STDIN_FILENO);
        dup2(pipeStdout[1], STDOUT_FILENO);
        dup2(pipeStderr[1], STDERR_FILENO);

        Log::clearChannels();

        // Prepare arguments
        std::string fullPath = uri.getFullPath();
        char *args[3] = {nullptr, nullptr, nullptr}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
        if (!cgiPath.empty())
        {
            args[0] = const_cast<char *>(cgiPath.c_str());  // NOLINT(cppcoreguidelines-pro-type-const-cast)
            args[1] = const_cast<char *>(fullPath.c_str()); // NOLINT(cppcoreguidelines-pro-type-const-cast)
        }
        else
        {
            args[0] = const_cast<char *>(fullPath.c_str()); // NOLINT(cppcoreguidelines-pro-type-const-cast)
        }
        execve(args[0], args, cgiEnv.toEnvp()); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        exit(1);
    }
    else
    {
        // Parent process
        auto cgiStdIn = std::make_unique<CgiSocket>(pipeStdin[1], ASocket::IoState::WRITE, "stdin");
        auto cgiStdOut = std::make_unique<CgiSocket>(pipeStdout[0], ASocket::IoState::READ, "stdout");
        auto cgiStdErr = std::make_unique<CgiSocket>(pipeStderr[0], ASocket::IoState::READ, "stderr");

        close(pipeStdin[0]);
        close(pipeStdout[1]);
        close(pipeStderr[1]);

        Log::debug("CGI process forked with PID: " + std::to_string(pid_));

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
        int status = 0;
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