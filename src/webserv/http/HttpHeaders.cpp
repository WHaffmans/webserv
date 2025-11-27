#include <webserv/http/HttpConstants.hpp> // for CRLF
#include <webserv/http/HttpHeaders.hpp>   // for HttpHeaders
#include <webserv/log/Log.hpp>
#include <webserv/utils/utils.hpp> // for trim

#include <algorithm> // for __transform_fn, transform
#include <cctype>    // for tolower
#include <string>
#include <unordered_map>
#include <utility> // for pair

std::optional<size_t> HttpHeaders::getContentLength() const
{
    const auto &value = this->get("Content-Length");
    if (value.empty())
    {
        return std::nullopt;
    }
    return utils::stoul(value);
}

std::optional<std::string> HttpHeaders::getContentType() const noexcept
{
    const auto &value = this->get("Content-Type");
    if (value.empty())
    {
        return std::nullopt;
    }
    return value;
}

std::optional<std::string> HttpHeaders::getHost() const noexcept
{
    const auto &value = this->get("Host");
    if (value.empty())
    {
        return std::nullopt;
    }
    return value;
}

void HttpHeaders::add(const std::string &name, const std::string &value) noexcept
{
    std::string lower = name;
    std::ranges::transform(lower, lower.begin(), ::tolower);
    if (!originalCase_.contains(lower))
    {
        originalCase_[lower] = name;
    }
    headers_[lower].push_back(value);
}

void HttpHeaders::remove(const std::string &name) noexcept
{
    std::string lower = name;
    std::ranges::transform(lower, lower.begin(), ::tolower);
    headers_.erase(lower);
    originalCase_.erase(lower);
}

const std::string &HttpHeaders::get(const std::string &name) const noexcept
{
    std::string lower = name;
    std::ranges::transform(lower, lower.begin(), ::tolower);
    auto it = headers_.find(lower);
    if (it != headers_.end() && !it->second.empty())
    {
        return it->second.front();
    }
    static const std::string empty;
    return empty;
}

bool HttpHeaders::has(const std::string &name) const noexcept
{
    std::string lower = name;
    std::ranges::transform(lower, lower.begin(), ::tolower);
    return headers_.contains(lower);
}

bool HttpHeaders::parse(const std::string &rawHeaders) noexcept
{
    Log::trace(LOCATION);
    size_t start = 0;
    size_t end = rawHeaders.find(Http::Protocol::CRLF);
    size_t headerCount = 0;

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

            // Reject headers with empty names
            if (name.empty())
            {
                Log::warning("Malformed header line (empty header name): " + line);
                return false;
            }

            // Validate header name characters (RFC 7230: field-name must be a token)
            // Token characters: alphanumeric, !, #, $, %, &, ', *, +, -, ., ^, _, `, |, ~
            for (char c : name)
            {
                if (std::isalnum(static_cast<unsigned char>(c)) == 0 && c != '!' && c != '#' && c != '$' && c != '%'
                    && c != '&' && c != '\'' && c != '*' && c != '+' && c != '-' && c != '.' && c != '^' && c != '_'
                    && c != '`' && c != '|' && c != '~')
                {
                    Log::warning("Malformed header line (invalid character in header name): " + line);
                    return false;
                }
            }

            // Reject values that start with ':' (e.g., "Badly-Formed:: value")
            if (!value.empty() && value.front() == ':')
            {
                Log::warning("Malformed header line (value starts with colon): " + line);
                return false;
            }

            // Enforce per-header value size limit
            if (value.size() > Http::Protocol::MAX_HEADER_SIZE)
            {
                Log::warning("Header value exceeds maximum size (" + std::to_string(value.size()) + ") for: " + name);
                return false;
            }

            // Enforce maximum number of headers
            ++headerCount;
            if (headerCount > Http::Protocol::MAX_HEADER_COUNT)
            {
                Log::warning("Too many headers: " + std::to_string(headerCount));
                return false;
            }

            this->add(name, value);
        }
        else if (!line.empty())
        {
            // Malformed header line (no colon) - this is an error
            Log::warning("Malformed header line (missing colon): " + line);
            return false;
        }
        start = end + Http::Protocol::CRLF.size();
        end = rawHeaders.find(Http::Protocol::CRLF, start);
    }
    return true;
}

const std::unordered_map<std::string, std::vector<std::string>> &HttpHeaders::getAll() const noexcept
{
    return headers_;
}

std::string HttpHeaders::toString() const noexcept
{
    std::string result;
    for (const auto &pair : headers_)
    {
        // Use original case for output (looks up from originalCase_ map)
        const std::string &outputName = originalCase_.contains(pair.first) ? originalCase_.at(pair.first) : pair.first;
        // Emit each value on its own header line
        for (const auto &val : pair.second)
        {
            result += outputName;
            result += ": ";
            result += val;
            result += "\r\n";
        }
    }
    result += "\r\n";
    return result;
}