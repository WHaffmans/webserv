#include <webserv/http/HttpResponse.hpp>

#include <webserv/http/HttpConstants.hpp> // for getStatusCodeReason

#include <iomanip>
#include <string> // for basic_string, operator+, string, char_traits, to_string
#include <vector> // for vector

HttpResponse::HttpResponse() : headers_(std::make_unique<HttpHeaders>()) {}

void HttpResponse::addHeader(const std::string &key, const std::string &value)
{
    headers_->add(key, value);
}

void HttpResponse::appendBody(const std::vector<uint8_t> &data)
{
    if (complete_)
    {
        Log::warning("Attempt to set body on a completed HttpResponse");
        return;
    }
    body_.insert(body_.end(), data.begin(), data.end());
}

void HttpResponse::appendBody(const std::string &body)
{
    if (complete_)
    {
        Log::warning("Attempt to set body on a completed HttpResponse");
        return;
    }
    body_.insert(body_.end(), body.begin(), body.end());
}

void HttpResponse::setBody(const std::vector<uint8_t> &data)
{
    if (complete_)
    {
        Log::warning("Attempt to set body on a completed HttpResponse");
        return;
    }
    body_ = data;
    setComplete();
}

void HttpResponse::setBody(const std::string &body)
{
    body_.assign(body.begin(), body.end());
    setComplete();
}

void HttpResponse::setStatus(uint16_t statusCode)
{
    statusCode_ = statusCode;
}

void HttpResponse::setComplete()
{
    complete_ = true;
}

void HttpResponse::setError(uint16_t statusCode)
{
    statusCode_ = statusCode;
    complete_ = true;
}

bool HttpResponse::isComplete() const noexcept
{
    return complete_;
}

const HttpHeaders &HttpResponse::getHeaders() const noexcept
{
    return *headers_;
}

std::string HttpResponse::getContentLengthHeader() const
{
    return "Content-Length: " + std::to_string(body_.size()) + "\r\n";
}

uint16_t HttpResponse::getStatusCode() const noexcept
{
    return statusCode_;
}

std::string HttpResponse::getDateHeader()
{
    time_t now = time(nullptr);
    struct tm *gmt = gmtime(&now);

    std::ostringstream oss;
    oss << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");

    return "Date: " + oss.str() + "\r\n";
}

std::vector<uint8_t> HttpResponse::toBytes(long offset) const
{
    std::string headerStr;
    std::string reason;

    reason = Http::getStatusCodeReason(statusCode_);

    headerStr = "HTTP/1.1 " + std::to_string(statusCode_) + " " + reason + "\r\n"; // todo: status line
    headerStr += getContentLengthHeader();
    headerStr += getDateHeader();
    headerStr += "Connection: close\r\n";
    headerStr += "Server: Webserv/1.0\r\n";

    headerStr += headers_->toString();

    std::vector<uint8_t> responseData(headerStr.begin(), headerStr.end());
    responseData.insert(responseData.end(), body_.begin(), body_.end());

    return {responseData.begin() + offset, responseData.end()};
}