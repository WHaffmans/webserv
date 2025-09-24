#include "webserv/http/HttpHeaders.hpp"

#include "webserv/config/utils.hpp"
#include "webserv/http/HttpConstants.hpp"

#include <algorithm>

std::optional<size_t> HttpHeaders::getContentLength() const
{
    const auto &value = get("Content-Length");
    if (value.empty())
    {
        return std::nullopt;
    }
    try
    {
        return std::stoul(value);
    }
    catch (...)
    {
        return std::nullopt;
    }
}

void HttpHeaders::add(const std::string &name, const std::string &value) // NOLINT(bugprone-easily-swappable-parameters)
{

    std::string lower = name;
    std::ranges::transform(lower, lower.begin(), ::tolower);
    headers_[lower] = value;
}

void HttpHeaders::remove(const std::string &name)
{
    headers_.erase(name);
}

const std::string &HttpHeaders::get(const std::string &name) const
{
    auto it = headers_.find(name);
    if (it != headers_.end())
    {
        return it->second;
    }
    static const std::string empty;
    return empty;
}

bool HttpHeaders::has(const std::string &name) const
{
    return headers_.contains(name);
}

void HttpHeaders::parse(const std::string &rawHeaders)
{
    size_t start = 0;
    size_t end = rawHeaders.find(Http::Protocol::CRLF);

    while (end != std::string::npos)
    {
        std::string line = rawHeaders.substr(start, end - start);
        size_t col = line.find(':');
        if (col != std::string::npos)
        {
            std::string name = line.substr(0, col);
            std::string value = line.substr(col + 1);
            name = trim(name);
            value = trim(value);
            this->add(name, value);
        }
        start = end + Http::Protocol::CRLF.size();
        end = rawHeaders.find(Http::Protocol::CRLF, start);
    }
}

std::string HttpHeaders::toString() const
{
    std::string result;
    for (const auto &pair : headers_)
    {
        result += pair.first + ": " + pair.second + "\r\n";
    }
    result += "\r\n";
    return result;
}