#include "webserv/http/HttpConstants.hpp"

#include <webserv/http/HttpResponse.hpp>

#include <string>
#include <vector>

HttpResponse::HttpResponse(Client *client) : client_(client), headers_(std::make_unique<HttpHeaders>()) {}

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

void HttpResponse::setStatus(int statusCode)
{
    statusCode_ = statusCode;
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