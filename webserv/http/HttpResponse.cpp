#include <webserv/http/HttpResponse.hpp>

#include <webserv/http/HttpConstants.hpp> // for getStatusCodeReason

#include <string> // for basic_string, operator+, string, char_traits, to_string
#include <vector> // for vector

HttpResponse::HttpResponse() : headers_(std::make_unique<HttpHeaders>()) {}

void HttpResponse::addHeader(const std::string &key, const std::string &value)
{
    headers_->add(key, value);
}

void HttpResponse::appendBody(const std::vector<uint8_t> &data)
{
    body_.insert(body_.end(), data.begin(), data.end());
}

void HttpResponse::appendBody(const std::string &body)
{
    body_.insert(body_.end(), body.begin(), body.end());
}

void HttpResponse::setBody(const std::vector<uint8_t> &data)
{
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

bool HttpResponse::isComplete() const
{
    return complete_;
}

const HttpHeaders &HttpResponse::getHeaders() const
{
    return *headers_;
}

std::string HttpResponse::getContentLength() const
{
    return "Content-Length: " + std::to_string(body_.size()) + "\r\n";
}

std::vector<uint8_t> HttpResponse::toBytes() const
{
    std::string headerStr;
    std::string reason;

    reason = Http::getStatusCodeReason(statusCode_);

    headerStr = "HTTP/1.1 " + std::to_string(statusCode_) + " " + reason + "\r\n"; // todo: status line
    headerStr += getContentLength();
    headerStr += headers_->toString();

    std::vector<uint8_t> responseData(headerStr.begin(), headerStr.end());
    responseData.insert(responseData.end(), body_.begin(), body_.end());

    return responseData;
}