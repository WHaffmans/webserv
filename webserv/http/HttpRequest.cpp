#include <webserv/http/HttpRequest.hpp>
#include <webserv/log/Log.hpp>

HttpRequest::HttpRequest(const ServerConfig *serverConfig, const Client *client)
    : serverConfig_(serverConfig), client_(client)
{
    Log::trace("HttpRequest constructor called");
}
HttpRequest::~HttpRequest()
{
    Log::trace("HttpRequest destructor called");
}

HttpRequest::State HttpRequest::getState() const
{
    Log::trace("HttpRequest::getState() called");
    return state_;
}

const std::string &HttpRequest::getHeaders() const
{
    return headers_;
}

const std::string &HttpRequest::getBody() const
{
    return body_;
}

size_t HttpRequest::getContentLength() const
{
    return contentLength_;
}

void HttpRequest::receiveData(const char *data, size_t length)
{
    Log::trace("HttpRequest::receiveData() called");
    buffer_.append(data, length);
    parseBuffer();
}

void HttpRequest::parseContentLength()
{
    // Parse headers to find Content-Length
    size_t pos = headers_.find("Content-Length:");
    if (pos != std::string::npos)
    {
        pos += 15;
        while (pos < headers_.size() && (headers_[pos] == ' ' || headers_[pos] == '\t'))
        {
            ++pos; // Skip whitespace
        }
        size_t endPos = headers_.find("\r\n", pos);
        std::string contentLengthValue = headers_.substr(pos, endPos - pos);
        contentLength_ = std::stoul(contentLengthValue);
    }
    else
    {
        contentLength_ = 0; // No body expected
    }
}

void HttpRequest::parseBuffer()
{
    while (true)
    {
        if (state_ == State::RequestLine)
        {
            size_t pos = buffer_.find("\r\n");
            if (pos == std::string::npos)
            {
                Log::debug("HttpRequest::parseBuffer() in state RequestLine waiting for more data");
                return; // Wait for more data
            }
            requestLine_ = buffer_.substr(0, pos);
            buffer_.erase(0, pos + 2);
            state_ = State::Headers;
        }
        else if (state_ == State::Headers)
        {
            size_t pos = buffer_.find("\r\n\r\n");
            if (pos == std::string::npos)
            {
                Log::debug("HttpRequest::parseBuffer() in state Headers waiting for more data");
                return; // Wait for more data
            }
            headers_ = buffer_.substr(0, pos + 2); // Include the last \r\n
            buffer_.erase(0, pos + 4);
            parseContentLength();

            if (contentLength_ > 0)
            {
                state_ = State::Body;
            }
            else
            {
                Log::debug("HttpRequest::parseBuffer() in state Headers no body to read");
                state_ = State::Complete;
                return; // No body to read
            }
        }
        else if (state_ == State::Body)
        {
            if (buffer_.size() < contentLength_)
            {
                Log::debug("HttpRequest::parseBuffer() in state Body waiting for more data");
                return; // Wait for more data
            }
            body_ = buffer_.substr(0, contentLength_);
            buffer_.erase(0, contentLength_);
            state_ = State::Complete;
        }
        else if (state_ == State::Complete)
        {
            Log::debug("HttpRequest::parseBuffer() request is complete");
            return; // Request is complete
        }
    }
}

void HttpRequest::reset()
{
    Log::trace("HttpRequest::reset() called");
    state_ = State::RequestLine;
    buffer_.clear();
    requestLine_.clear();
    headers_.clear();
    body_.clear();
    contentLength_ = 0;
}
