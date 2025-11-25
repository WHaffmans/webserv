#include "webserv/http/HttpConstants.hpp"
#include "webserv/main.hpp"

#include <webserv/client/Client.hpp> // for Client
#include <webserv/handler/CgiHandler.hpp>
#include <webserv/handler/CgiProcess.hpp>   // for CgiProcess
#include <webserv/handler/ErrorHandler.hpp> // for ErrorHandler
#include <webserv/handler/URI.hpp>          // for URI
#include <webserv/http/HttpRequest.hpp>     // for HttpRequest
#include <webserv/http/HttpResponse.hpp>    // for HttpResponse
#include <webserv/log/Log.hpp>              // for Log, LOCATION
#include <webserv/socket/CgiSocket.hpp>     // for CgiSocket
#include <webserv/socket/TimerSocket.hpp>   // for TimerSocket
#include <webserv/utils/utils.hpp>          // for trim

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <functional> // for function
#include <optional>
#include <string>
#include <utility> // for move

#include <sys/types.h> // for ssize_t

CgiHandler::CgiHandler(const HttpRequest &request, HttpResponse &response)
    : AHandler(request, response), cgiProcess_(nullptr), cgiStdIn_(nullptr), cgiStdOut_(nullptr)
{
    Log::debug("CgiHandler constructed");
}

void CgiHandler::handle()
{
    Log::info("CgiHandler handling request");

    if (request_.getUri().isCgi() && request_.getUri().getCgiPath().empty()
        && access(request_.getUri().getFullPath().c_str(), X_OK) != 0)
    {
        ErrorHandler::createErrorResponse(403, response_);
        return;
    }
    // Initialize CGI process
    cgiProcess_ = std::make_unique<CgiProcess>(request_, *this);

    startTimer();

    Log::info("CGI process started and sockets registered");
}

static inline bool findHeaderEnd(const std::string &s, size_t &pos, long &sepSize)
{
    Log::trace(LOCATION);
    size_t a = s.find("\r\n\r\n");
    size_t b = s.find("\n\n");
    size_t c = s.find("\r\r");
    size_t end = std::min({a, b, c});

    if (end == std::string::npos)
    {
        return false;
    }
    sepSize = (end == a) ? 4 : 2;
    pos = end;
    return true;
}

void CgiHandler::write()
{
    Log::trace(LOCATION);
    if (cgiStdIn_ == nullptr)
    {
        Log::error("CGI stdin socket is null");
        return;
    }
    const std::string &body = request_.getBody();

    if (writeOffset_ < body.size())
    {
        const char *data = body.data() + writeOffset_; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        size_t remaining = body.size() - writeOffset_;
        size_t chunk = remaining > Constants::CHUNK_SIZE ? Constants::CHUNK_SIZE : remaining;
        ssize_t bytesWritten = cgiStdIn_->write(data, chunk);
        if (bytesWritten > 0)
        {
            writeOffset_ += static_cast<size_t>(bytesWritten);
            Log::debug("Wrote " + std::to_string(bytesWritten) + " bytes, write offset " + std::to_string(writeOffset_)
                       + "/ " + std::to_string(body.size()));
        }
    }

    if (writeOffset_ >= body.size())
    {
        Log::debug("CGI stdin sent " + std::to_string(body.size()) + " bytes, closing write end");
        request_.getClient().removeSocket(cgiStdIn_.get());
        cgiStdIn_.reset();
    }
}

void CgiHandler::read()
{
    Log::trace(LOCATION);
    if (cgiStdOut_ == nullptr)
    {
        Log::debug("CGI stdout socket is null in read()");
        return;
    }

    char buffer[Constants::BUFFER_SIZE] = {};
    ssize_t bytesRead = cgiStdOut_->read(buffer, sizeof(buffer));

    if (bytesRead > 0)
    {
        appendToBuffer(buffer, static_cast<size_t>(bytesRead));
        Log::debug("Read " + std::to_string(bytesRead)
                   + " bytes from CGI stdout (buffer size: " + std::to_string(buffer_.size()) + ")");

        // Parse headers once, as soon as we have them
        if (!headersParsed_)
        {
            size_t headerEnd = 0;
            long sepSize = 0;
            std::string snapshot(buffer_.begin(), buffer_.end());
            if (findHeaderEnd(snapshot, headerEnd, sepSize))
            {
                std::string headers(snapshot.begin(), snapshot.begin() + static_cast<long>(headerEnd));
                parseCgiHeaders(headers);
                // After headers parsed, remove them from buffer_ so it contains only body
                buffer_.erase(buffer_.begin(), buffer_.begin() + static_cast<long>(headerEnd) + sepSize);
                headersParsed_ = true;
                contentLength_ = response_.getHeaders().getContentLength();
                Log::debug("CGI headers parsed, Content-Length: "
                           + (contentLength_.has_value() ? std::to_string(contentLength_.value()) : "not set"));
            }
        }

        // Only finalize if we've finished writing the request body AND we have complete response
        bool responseComplete = false;
        if (headersParsed_ && contentLength_.has_value())
        {
            responseComplete = (buffer_.size() >= contentLength_.value());
        }

        if (responseComplete)
        {
            Log::debug("Response complete: headers parsed and content received");
            request_.getClient().removeSocket(cgiStdOut_.get());
            cgiStdOut_.reset();
            finalizeCgiResponse();
            return;
        }
        return;
    }

    if (bytesRead == 0)
    {
        // EOF from CGI process
        Log::info("CGI process closed stdout, fd: " + std::to_string(cgiStdOut_->getFd()));
        request_.getClient().removeSocket(cgiStdOut_.get());
        cgiStdOut_.reset();

        // If headers not parsed yet, try once more
        if (!headersParsed_)
        {
            size_t headerEnd = 0;
            long sep = 0;
            std::string snap(buffer_.begin(), buffer_.end());
            if (findHeaderEnd(snap, headerEnd, sep))
            {
                std::string headers(snap.begin(), snap.begin() + static_cast<long>(headerEnd));
                parseCgiHeaders(headers);
                buffer_.erase(buffer_.begin(), buffer_.begin() + static_cast<long>(headerEnd) + sep);
                headersParsed_ = true;
            }
        }

        // Only finalize if we've finished writing the request body
        finalizeCgiResponse();
        return;
    }

    if (bytesRead < 0)
    {

        Log::error("Error reading from CGI stdout: " + std::string(strerror(errno)));
        finalizeCgiResponse();
    }
}

void CgiHandler::error()
{
    Log::trace(LOCATION);
    if (cgiStdErr_ == nullptr)
    {
        return;
    }
    while (true)
    {
        char buffer[Constants::BUFFER_SIZE] = {};
        ssize_t bytesRead = cgiStdErr_->read(buffer, sizeof(buffer));
        if (bytesRead > 0)
        {
            appendToBuffer(buffer, static_cast<size_t>(bytesRead));
            Log::error("CGI stderr output (fd: " + std::to_string(cgiStdErr_->getFd())
                       + "): " + std::string(buffer, static_cast<size_t>(bytesRead)));
            continue;
        }
        if (bytesRead == 0)
        {
            Log::info("CGI process closed stderr, fd: " + std::to_string(cgiStdErr_->getFd()));
            request_.getClient().removeSocket(cgiStdErr_.get());
            cgiStdErr_.reset();
            break;
        }
        break;
    }
}

void CgiHandler::setCgiSockets(std::unique_ptr<CgiSocket> cgiStdIn, std::unique_ptr<CgiSocket> cgiStdOut,
                               std::unique_ptr<CgiSocket> cgiStdErr)
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

    if (request_.getBody().empty())
    {
        request_.getClient().removeSocket(cgiStdIn_.get());
        cgiStdIn_.reset();
    }
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
    if (headersParsed_)
    {
        return;
    }
    size_t headerEnd = 0;
    long sepSize = 0;
    std::string header(buffer_.begin(), buffer_.end());
    if (!findHeaderEnd(header, headerEnd, sepSize))
    {
        Log::debug("CGI output headers not complete yet");
        return;
    }
    std::string headers(header.begin(), header.begin() + static_cast<long>(headerEnd));
    Log::debug("CGI output headers: " + headers);
    parseCgiHeaders(headers);
    buffer_.erase(buffer_.begin(), buffer_.begin() + static_cast<long>(headerEnd) + sepSize);
    headersParsed_ = true;
    contentLength_ = response_.getHeaders().getContentLength();
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

    contentLength_ = response_.getHeaders().getContentLength();
}

void CgiHandler::handleTimeout()
{
    Log::warning("CGI handler timeout occurred for PID: " + std::to_string(pid_));
    char buffer[9] = {}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    ssize_t bytesRead = timerSocket_->read(buffer, sizeof(buffer) - 1);
    buffer[bytesRead] = '\0';
    if (bytesRead <= 0)
    {
        // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        return;
    }
    // Terminate the CGI process if it's still running
    if (cgiProcess_)
    {
        cgiProcess_->kill();
        Log::info("Terminated CGI process with PID: " + std::to_string(pid_));
    }

    ErrorHandler::createErrorResponse(Http::StatusCode::GATEWAY_TIMEOUT, response_);
    // cancelTimer();
}

void CgiHandler::finalizeCgiResponse()
{
    Log::trace(LOCATION);
    auto status = response_.getHeaders().get("Status");
    wait();
    if (cgiProcess_ && cgiProcess_->getExitCode() > 0 && status.empty())
    {
        response_.setStatus(500);
    }
    else if (!status.empty())
    {
        response_.setStatus(std::atoi(status.c_str()));
    }
    response_.appendBody(buffer_);
    response_.setComplete();
    buffer_.clear();
}

void CgiHandler::appendToBuffer(const char *data, size_t length)
{
    Log::trace(LOCATION);
    buffer_.insert(buffer_.end(), data, data + length);
}