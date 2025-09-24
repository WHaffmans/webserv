#include <webserv/client/Client.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/http/HttpConstants.hpp>
#include <webserv/http/HttpRequest.hpp>
#include <webserv/log/Log.hpp>

#include <sstream>
#include <vector>

HttpRequest::HttpRequest(const ServerConfig *serverConfig, const Client *client)
    : serverConfig_(serverConfig), client_(client)
{
    Log::trace(LOCATION);
}

HttpRequest::~HttpRequest()
{
    Log::trace(LOCATION);
}

HttpRequest::State HttpRequest::getState() const
{
    Log::trace(LOCATION);
    return state_;
}

const HttpHeaders &HttpRequest::getHeaders() const
{
    return headers_;
}

const std::string &HttpRequest::getBody() const
{
    return body_;
}

void HttpRequest::receiveData(const char *data, size_t length)
{
    if (buffer_.size() + length > Http::Protocol::MAX_HEADER_SIZE &&
        state_ == State::Headers) // !: This is a define but should be configurable
    {
        Log::warning("Request size exceeds maximum limit of " + std::to_string(Http::Protocol::MAX_HEADER_SIZE) +
                     " bytes");
        state_ = State::Complete; // Mark as complete to avoid further processing
        return;
    }
    Log::trace(LOCATION);
    buffer_.append(data, length);
    parseBuffer();
}

void HttpRequest::parseBuffer()
{
    while (true)
    {
        switch (state_)
        {
        case State::RequestLine:
            if (!parseBufferforRequestLine())
            {
                return; // Wait for more data
            }
            break;
        case State::Headers:
            if (!parseBufferforHeaders())
            {
                return; // Wait for more data
            }
            break;
        case State::Body:
            if (!parseBufferforBody())
            {
                return; // Wait for more data
            }
            break;
        case State::Chunked:
            if (!parseBufferforChunkedBody())
            {
                return; // Wait for more data
            }
            break;
        case State::Complete:
            Log::debug("HttpRequest::parseBuffer() request is complete");
            return; // Request is complete
        }
    }
}

bool HttpRequest::parseBufferforRequestLine()
{
    Log::trace(LOCATION);
    size_t pos = buffer_.find(Http::Protocol::CRLF);
    if (pos == std::string::npos)
    {
        Log::debug("RequestLine waiting for more data : " + LOCATION);
        return false;
    }
    std::string requestLine_ = buffer_.substr(0, pos);

    buffer_.erase(0, pos + Http::Protocol::CRLF.size());
    state_ = State::Headers;

    std::vector<std::string> parts;
    std::string part;
    std::stringstream ss(requestLine_);
    while (ss >> part)
    {
        parts.push_back(part);
    }
    if (parts.size() != 3)
    {
        Log::warning("Invalid request line: " + requestLine_);
        state_ = State::Complete; // Mark as complete to avoid further processing
        return true;
    }
    method_ = parts[0];
    target_ = parts[1];
    httpVersion_ = parts[2];
    Log::debug("Parsed Request Line: Method=" + method_ + " Target=" + target_ + " Version=" + httpVersion_);
    return true;
}

bool HttpRequest::parseBufferforHeaders()
{
    Log::trace(LOCATION);
    size_t pos = buffer_.find(Http::Protocol::DOUBLE_CRLF);
    if (pos == std::string::npos)
    {
        Log::debug("Headers waiting for more data: " + LOCATION);
        return false; // Wait for more data
    }
    // headers_ = buffer_.substr(0, pos + Http::Protocol::CRLF.size()); // Include the last \r\n
    headers_.parse(buffer_.substr(0, pos + Http::Protocol::CRLF.size()));
    buffer_.erase(0, pos + Http::Protocol::DOUBLE_CRLF.size());
    // parseContentLength();

    if (this->headers_.getContentLength() > 0)
    {
        state_ = State::Body;
        return true;
    }
    if (this->headers_.has("Transfer-Encoding") && this->headers_.get("Transfer-Encoding") == "chunked")
    {
        Log::debug("HttpRequest::parseBuffer() in state Headers with chunked encoding");
        state_ = State::Chunked;
        return true;
    }
    state_ = State::Complete;
    return false; // No body to read
}

bool HttpRequest::parseBufferforChunkedBody()
{
    Log::trace(LOCATION);
    while (true)
    {
        size_t pos = buffer_.find(Http::Protocol::CRLF);
        if (pos == std::string::npos)
        {
            Log::debug("Chunked body waiting for more data: " + LOCATION);
            return false;
        }
        std::string chunkSizeStr = buffer_.substr(0, pos);
        size_t chunkSize = 0;
        try
        {
            chunkSize = std::stoul(chunkSizeStr, nullptr, 16);
        }
        catch (const std::exception &e)
        {
            Log::warning("Invalid chunk size: " + chunkSizeStr + " (" + e.what() + ")");
            state_ = State::Complete; // Mark as complete to avoid further processing
            return true;
        }
        if (chunkSize == 0)
        {
            state_ = State::Complete; // Last chunk
            buffer_.erase(0, pos + Http::Protocol::CRLF.size());
            return true;
        }
        // TODO: Copilot added this but I don't understand why it's needed
        if (buffer_.size() < pos + Http::Protocol::CRLF.size() + chunkSize + Http::Protocol::CRLF.size())
        {
            Log::debug("Chunked body waiting for more data: " + LOCATION);
            return false;
        }
        body_ += buffer_.substr(pos + Http::Protocol::CRLF.size(), chunkSize);
        buffer_.erase(0, pos + Http::Protocol::CRLF.size() + chunkSize + Http::Protocol::CRLF.size());
    }
    return true;
}

bool HttpRequest::parseBufferforBody()
{
    if (!headers_.getContentLength().has_value())
    {
        Log::warning("HttpRequest::parseBuffer() in state Body but no Content-Length header found");
        state_ = State::Complete;
        return true;
    }
    Log::trace(LOCATION);
    if (buffer_.size() < *headers_.getContentLength())
    {
        Log::debug("Body waiting for more data: " + LOCATION);
        return false; // Wait for more data
    }
    body_ = buffer_.substr(0, *headers_.getContentLength());
    buffer_.erase(0, *headers_.getContentLength());
    state_ = State::Complete;

    return true;
}

void HttpRequest::reset()
{
    Log::trace(LOCATION);
    state_ = State::RequestLine;
    buffer_.clear();
    // requestLine_.clear();
    // headers_.clear();
    body_.clear();
    // contentLength_ = 0;
}
