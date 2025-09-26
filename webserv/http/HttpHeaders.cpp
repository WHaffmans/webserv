#include <webserv/config/utils.hpp>       // for trim
#include <webserv/http/HttpConstants.hpp> // for CRLF
#include <webserv/http/HttpHeaders.hpp>   // for HttpHeaders
#include <webserv/log/Log.hpp>
#include <algorithm> // for __transform_fn, transform
#include <cctype>    // for tolower
#include <utility>   // for pair

std::optional<size_t> HttpHeaders::getContentLength() const
{
    Log::trace(LOCATION);
    const auto &value = this->get("Content-Length");
    if (value.empty())
    {
        return std::nullopt;
    }
    return utils::stoul(value);
}

std::optional<std::string> HttpHeaders::getContentType() const
{
    Log::trace(LOCATION);
    const auto &value = this->get("Content-Type");
    if (value.empty())
    {
        return std::nullopt;
    }
    return value;
}

std::optional<std::string> HttpHeaders::getHost() const
{
    Log::trace(LOCATION);
    const auto &value = this->get("Host");
    if (value.empty())
    {
        return std::nullopt;
    }
    return value;
}

void HttpHeaders::add(const std::string &name, const std::string &value) // NOLINT(bugprone-easily-swappable-parameters)
{
    Log::trace(LOCATION);
    std::string lower = name;
    std::ranges::transform(lower, lower.begin(), ::tolower);
    headers_[lower] = value;
}

void HttpHeaders::remove(const std::string &name)
{
    Log::trace(LOCATION);
    headers_.erase(name);
}

const std::string &HttpHeaders::get(const std::string &name) const
{
    Log::trace(LOCATION);
    std::string lower = name;
    std::ranges::transform(lower, lower.begin(), ::tolower);
    auto it = headers_.find(lower);
    if (it != headers_.end())
    {
        return it->second;
    }
    static const std::string empty;
    return empty;
}

bool HttpHeaders::has(const std::string &name) const
{
    Log::trace(LOCATION);
    std::string lower = name;
    std::ranges::transform(lower, lower.begin(), ::tolower);
    return headers_.contains(lower);
}

void HttpHeaders::parse(const std::string &rawHeaders)
{
    Log::trace(LOCATION);
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
            name = utils::trim(name);
            value = utils::trim(value);
            this->add(name, value);
        }
        start = end + Http::Protocol::CRLF.size();
        end = rawHeaders.find(Http::Protocol::CRLF, start);
    }
}

std::string HttpHeaders::toString() const
{
    Log::trace(LOCATION);
    std::string result;
    for (const auto &pair : headers_)
    {
        result += pair.first + ": " + pair.second + "\r\n";
    }
    result += "\r\n";
    return result;
}