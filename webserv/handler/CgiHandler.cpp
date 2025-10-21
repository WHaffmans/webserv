#include <webserv/client/Client.hpp> // for Client
#include <webserv/handler/CgiHandler.hpp>
#include <webserv/handler/CgiProcess.hpp> // for CgiProcess
#include <webserv/http/HttpRequest.hpp>   // for HttpRequest
#include <webserv/http/HttpResponse.hpp>  // for HttpResponse
#include <webserv/log/Log.hpp>            // for Log
#include <webserv/socket/CgiSocket.hpp>   // for CgiSocket
#include <webserv/utils/utils.hpp>        // for stoul

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
        request_.getClient().removeSocket(cgiStdIn_.get());
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
    request_.getClient().removeSocket(cgiStdIn_.get());
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
        request_.getClient().removeSocket(cgiStdOut_.get());
        cgiStdOut_ = nullptr;
        parseCgiOutput();
        return;
    }
    else
    {
        buffer[bytesRead] = '\0'; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        appendToBuffer(buffer, static_cast<size_t>(bytesRead));
        Log::debug("Read " + std::to_string(bytesRead)
                   + " bytes from CGI stdout, fd: " + std::to_string(cgiStdOut_->getFd()));
    }
}

void CgiHandler::error()
{
    Log::trace(LOCATION);
    if (cgiStdErr_ == nullptr)
    {
        Log::error("CGI stderr socket is null");
        return;
    }
    char buffer[bufferSize_] = {}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    ssize_t bytesRead
        = cgiStdErr_->read(buffer, sizeof(buffer) - 1); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    if (bytesRead < 0)
    {
        Log::error("Failed to read from CGI stderr, fd: " + std::to_string(cgiStdErr_->getFd()));
    }
    else if (bytesRead == 0)
    {
        Log::info("CGI process closed stderr, fd: " + std::to_string(cgiStdErr_->getFd()));
        request_.getClient().removeSocket(cgiStdErr_.get());
        cgiStdErr_ = nullptr;
        return;
    }
    else
    {
        buffer[bytesRead] = '\0'; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        Log::error("CGI stderr output (fd: " + std::to_string(cgiStdErr_->getFd()) + "): "
                   + std::string(buffer, static_cast<size_t>(bytesRead)));
    }
}

void CgiHandler::setCgiSockets(std::unique_ptr<CgiSocket> cgiStdIn, std::unique_ptr<CgiSocket> cgiStdOut, std::unique_ptr<CgiSocket> cgiStdErr)
{
    cgiStdIn->setCallback([this]() { write(); });
    cgiStdOut->setCallback([this]() { read(); });
    cgiStdErr->setCallback([this]() { error(); });

    cgiStdOut_ = std::move(cgiStdOut);
    cgiStdIn_ = std::move(cgiStdIn);
    cgiStdErr_ = std::move(cgiStdErr);

    request_.getClient().addSocket(cgiStdIn_.get());
    request_.getClient().addSocket(cgiStdOut_.get());
    request_.getClient().addSocket(cgiStdErr_.get());
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

void CgiHandler::parseCgiOutput()
{
    Log::trace(LOCATION);

    // Parse the headers from the buffer
    size_t headerEnd = std::string(buffer_.begin(), buffer_.end()).find("\r\n\r\n");
    if (headerEnd == std::string::npos)
    {
        Log::debug("CGI output headers not complete yet");
        return;
    }

    // Parse the headers
    std::string headers(buffer_.begin(), buffer_.begin() + static_cast<long>(headerEnd));
    Log::debug("CGI output headers: " + headers);
    parseCgiHeaders(headers);

    buffer_.erase(buffer_.begin(), buffer_.begin() + static_cast<long>(headerEnd) + 4);
    parseCgiBody();
}

void CgiHandler::parseCgiHeaders(std::string &headers)
{
    Log::trace(LOCATION);

    // Debug: log the raw headers to see what we're getting
    Log::debug("Raw CGI headers (length=" + std::to_string(headers.length()) + "): [" + headers + "]");

    size_t start = 0;
    size_t end = headers.find("\r\n");
    while (end != std::string::npos)
    {
        std::string header = headers.substr(start, end - start);
        if (!header.empty())
        {
            Log::debug("CGI header: [" + header + "]");
            size_t colonPos = header.find(':');
            if (colonPos != std::string::npos)
            {
                std::string name = header.substr(0, colonPos);
                std::string value = header.substr(colonPos + 1);
                name = utils::trim(name);
                value = utils::trim(value);
                response_.addHeader(name, value);
            }
            else
            {
                Log::warning("CGI header has no colon: [" + header + "]");
            }
        }
        start = end + 2;
        end = headers.find("\r\n", start);
    }

    // Handle the last header (might not have trailing \r\n)
    std::string lastHeader = headers.substr(start);
    if (!lastHeader.empty())
    {
        Log::debug("Last CGI header: [" + lastHeader + "]");
        size_t colonPos = lastHeader.find(':');
        if (colonPos != std::string::npos)
        {
            std::string name = lastHeader.substr(0, colonPos);
            std::string value = lastHeader.substr(colonPos + 1);
            name = utils::trim(name);
            value = utils::trim(value);
            response_.addHeader(name, value);
        }
    }
}

void CgiHandler::parseCgiBody()
{
    Log::trace(LOCATION);

    // Append the body to the response
    response_.appendBody(buffer_);
    response_.setComplete();
    buffer_.clear();
}

void CgiHandler::appendToBuffer(const char *data, size_t length)
{
    Log::trace(LOCATION);
    buffer_.insert(buffer_.end(), data, data + length);
}