#include "webserv/http/HttpRequest.hpp"
#include "webserv/log/Log.hpp"
#include "webserv/utils/FileUtils.hpp"

#include <webserv/handler/CgiEnvironment.hpp>
#include <webserv/handler/URI.hpp>      // for URI
#include <webserv/http/HttpHeaders.hpp> // for HttpHeaders

#include <algorithm>
#include <cctype>
#include <cstring>    // for strcpy, size_t
#include <filesystem> // for absolute, path
#include <optional>   // for optional
#include <utility>    // for pair

#include <sys/stat.h>

CgiEnvironment::CgiEnvironment(const URI &uri, const HttpRequest &request)
{
    Log::trace(LOCATION);
    env_["GATEWAY_INTERFACE"] = "CGI/1.1";
    env_["SERVER_PROTOCOL"] = "HTTP/1.1";
    env_["REQUEST_METHOD"] = request.getMethod();
    env_["SCRIPT_NAME"] = FileUtils::joinPath(uri.getDir(), uri.getBaseName());
    env_["PHP_SELF"] = env_["SCRIPT_NAME"];
    env_["SCRIPT_FILENAME"] = uri.getFullPath(); // Full filesystem path to the script (required by PHP)
    env_["QUERY_STRING"] = uri.getQuery();
    env_["REQUEST_URI"] = request.getTarget();
    env_["PATH_INFO"] = uri.getPathInfo();
    if (uri.getConfig()->get<bool>("42_tester").value_or(false))
    {
        env_["PATH_INFO"] = request.getTarget(); // TODO This is only correct for the tester;
    }

    // Only set CONTENT_TYPE and CONTENT_LENGTH if they have values
    auto contentType = request.getHeaders().getContentType();
    if (contentType.has_value())
    {
        env_["CONTENT_TYPE"] = contentType.value();
    }
    auto contentLength = request.getHeaders().getContentLength();
    if (contentLength.has_value())
    {
        env_["CONTENT_LENGTH"] = std::to_string(contentLength.value());
    }

    std::string host_port = request.getHeaders().getHost().value_or("");
    size_t colonPos = host_port.find(':');
    std::string host = (colonPos != std::string::npos) ? host_port.substr(0, colonPos) : host_port;
    int port = (colonPos != std::string::npos) ? std::stoi(host_port.substr(colonPos + 1)) : 80;

    env_["SERVER_NAME"] = host;
    env_["SERVER_PORT"] = std::to_string(port);
    env_["REMOTE_ADDR"] = request.getClient().getClientAddress(); // Placeholder, should be set to actual remote address
    env_["REDIRECT_STATUS"] = "200";                              // Required by PHP with force-cgi-redirect enabled
    env_["SERVER_SOFTWARE"] = "Webserv/1.2";
    env_["REQUEST_SCHEME"] = "HTTP";
    env_["HTTP_VERSION"] = "1.1";
    // Add HTTP_ headers
    const HttpHeaders &headers = request.getHeaders();

    env_["HTTP_HOST"] = headers.getHost().value_or("localhost:8080"); // TODO: Default value for testing

    // Map common request headers to CGI environment variables
    addHttpHeaderToEnv("Cookie", headers, "; ");
    addHttpHeaderToEnv("User-Agent", headers);
    addHttpHeaderToEnv("Accept", headers);
    addHttpHeaderToEnv("Accept-Language", headers);
    addHttpHeaderToEnv("Accept-Encoding", headers);

    env_["UPLOAD_TMP_DIR"] = "./htdocs/tmp"; // Example upload directory, adjust as needed
    env_["TMP_DIR"] = "./htdocs/tmp";        // Example temp directory, adjust as needed

    appendCustomHeaders(headers);
}

char **CgiEnvironment::toEnvp() const
{
    char **envp = new char *[env_.size() + 1];
    size_t index = 0;
    for (auto it = env_.begin(); it != env_.end(); ++it, ++index)
    {
        std::string entry = it->first + "=" + it->second;
        envp[index] = new char[entry.size() + 1];
        std::strcpy(envp[index], entry.c_str());
    }
    envp[index] = nullptr; // Null-terminate the array
    return envp;
}

std::string CgiEnvironment::get(const std::string &key) const
{
    auto it = env_.find(key);
    if (it != env_.end())
    {
        return it->second;
    }
    return "";
}

void CgiEnvironment::addHttpHeaderToEnv(const std::string &headerName, const HttpHeaders &headers,
                                        const char *separator)
{
    std::string lower = headerName;
    std::ranges::transform(lower, lower.begin(), ::tolower);
    auto it = headers.getAll().find(lower);
    if (it == headers.getAll().end() || it->second.empty())
    {
        return;
    }

    // Build HTTP_ environment variable name (e.g., "Cookie" -> "HTTP_COOKIE")
    std::string envKey = "HTTP_" + headerName;
    std::ranges::transform(envKey, envKey.begin(), ::toupper);
    std::replace(envKey.begin(), envKey.end(), '-', '_');

    // Join multiple header values
    const std::vector<std::string> &vals = it->second;
    std::string joined;
    for (size_t i = 0; i < vals.size(); ++i)
    {
        if (i != 0)
        {
            joined += separator;
        }
        joined += vals[i];
    }

    env_[envKey] = joined;
}

void CgiEnvironment::appendCustomHeaders(const HttpHeaders &headers)
{
    Log::trace(LOCATION);
    for (const auto &header : headers.getAll())
    {
        // header.first is stored lower-cased by HttpHeaders
        if (!header.first.starts_with("x-"))
        {
            continue;
        }
        std::string key = "HTTP_" + header.first;
        std::transform(key.begin(), key.end(), key.begin(), ::toupper);
        std::replace(key.begin(), key.end(), '-', '_');
        // Join multiple header values with a comma (RFC: combine field-values where appropriate)
        std::string joined;
        for (size_t i = 0; i < header.second.size(); ++i)
        {
            if (i != 0)
            {
                joined += ", ";
            }
            joined += header.second[i];
        }
        env_[key] = joined;
        std::string msg = "Added custom header with key: ";
        msg += key;
        msg += " And value: ";
        msg += joined;
        Log::debug(msg);
    }
}