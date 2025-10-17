#include <webserv/client/Client.hpp> // for Client
#include <webserv/handler/CgiHandler.hpp>
#include <webserv/handler/CgiProcess.hpp> // for CgiProcess
#include <webserv/http/HttpRequest.hpp>   // for HttpRequest
#include <webserv/http/HttpResponse.hpp>  // for HttpResponse
#include <webserv/log/Log.hpp>            // for Log
#include <webserv/socket/CgiSocket.hpp>   // for CgiSocket

CgiHandler::CgiHandler(const HttpRequest &request, HttpResponse &response)
    : AHandler(request, response), cgiProcess_(nullptr), cgiStdIn_(nullptr), cgiStdOut_(nullptr)
{
    Log::debug("CgiHandler constructed");
}

void CgiHandler::handle()
{
    Log::info("CgiHandler handling request");

    // Initialize CGI process
    cgiProcess_ = std::make_unique<CgiProcess>(request_, *this);


    Log::info("CGI process started and sockets registered");
}

void CgiHandler::write()
{
    Log::trace(LOCATION);
    if (cgiStdIn_ == nullptr)
    {
        Log::error("CGI stdin socket is null");
        return;
    }
    if (request_.getBody().empty())
    {
        Log::debug("No body to write to CGI stdin, fd: " + std::to_string(cgiStdIn_->getFd()));
        request_.getClient().removeCgiSocket(cgiStdIn_.get());
        cgiStdIn_ = nullptr;
        return;
    }
    ssize_t bytesWritten = cgiStdIn_->write(request_.getBody().data(), request_.getBody().size());
    if (bytesWritten < 0)
    {
        Log::error("Failed to write to CGI stdin, fd: " + std::to_string(cgiStdIn_->getFd()));
    }
    else
    {
        Log::debug("Wrote " + std::to_string(bytesWritten)
                   + " bytes to CGI stdin, fd: " + std::to_string(cgiStdIn_->getFd()));
    }
    request_.getClient().removeCgiSocket(cgiStdIn_.get());
    cgiStdIn_ = nullptr;
}

void CgiHandler::read()
{
    Log::trace(LOCATION);
    if (cgiStdOut_ == nullptr)
    {
        Log::error("CGI stdout socket is null");
        return;
    }
    char buffer[bufferSize_] = {}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    ssize_t bytesRead
        = cgiStdOut_->read(buffer, sizeof(buffer) - 1); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    if (bytesRead < 0)
    {
        Log::error("Failed to read from CGI stdout, fd: " + std::to_string(cgiStdOut_->getFd()));
    }
    else if (bytesRead == 0)
    {
        Log::info("CGI process closed stdout, fd: " + std::to_string(cgiStdOut_->getFd()));
        request_.getClient().removeCgiSocket(cgiStdOut_.get());
        cgiStdOut_ = nullptr;
        response_.addHeader("Content-Type", "text/html");
        response_.setComplete();
        return;
    }
    else
    {
        buffer[bytesRead] = '\0'; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        response_.appendBody(std::string(buffer, static_cast<size_t>(bytesRead)));
        Log::debug("Read " + std::to_string(bytesRead)
                   + " bytes from CGI stdout, fd: " + std::to_string(cgiStdOut_->getFd()));
    }
}

void CgiHandler::setCgiSockets(std::unique_ptr<CgiSocket> cgiStdIn, std::unique_ptr<CgiSocket> cgiStdOut)
{
    cgiStdIn->setCallback([this]() { write(); });
    cgiStdOut->setCallback([this]() { read(); });

    cgiStdOut_ = std::move(cgiStdOut);
    cgiStdIn_ = std::move(cgiStdIn);

    request_.getClient().setCgiSockets(cgiStdIn_.get(), cgiStdOut_.get()); // write

    // TODO add to handler
}

void CgiHandler::wait() noexcept
{
    if (cgiProcess_)
    {
        cgiProcess_->wait();
    }
}

void CgiHandler::setPid(int pid)
{
    pid_ = pid;

}