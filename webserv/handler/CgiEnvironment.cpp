#include "webserv/http/HttpRequest.hpp"
#include "webserv/log/Log.hpp"
#include "webserv/utils/FileUtils.hpp"
#include "webserv/utils/utils.hpp"

#include <webserv/handler/CgiEnvironment.hpp>
#include <webserv/handler/URI.hpp>      // for URI
#include <webserv/http/HttpHeaders.hpp> // for HttpHeaders

#include <algorithm>
#include <cctype>
#include <cstring>  // for strcpy, size_t
#include <optional> // for optional
#include <utility>  // for pair

#include <sys/stat.h>

CgiEnvironment::CgiEnvironment(const URI &uri, const HttpRequest &request)
{
    Log::trace(LOCATION);

    std::string host_port = request.getHeaders().getHost().value_or("");
    size_t colonPos = host_port.find(':');
    std::string host = (colonPos != std::string::npos) ? host_port.substr(0, colonPos) : host_port;
    int port = (colonPos != std::string::npos) ? std::stoi(host_port.substr(colonPos + 1)) : 80;

    std::string root = uri.getConfig()->get<std::string>("root").value();
    env_["GATEWAY_INTERFACE"] = "CGI/1.1";
    env_["SERVER_PROTOCOL"] = "HTTP/1.1";
    env_["REQUEST_METHOD"] = request.getMethod();
    env_["SCRIPT_NAME"] = uri.getFullPath();
    env_["PHP_SELF"] = env_["SCRIPT_NAME"];
    env_["SCRIPT_FILENAME"] = uri.getFullPath();
    env_["QUERY_STRING"] = uri.getQuery();
    env_["REQUEST_URI"] = uri.isDirectory() ? utils::ensureTrailingSlash(request.getTarget()) : request.getTarget();
    env_["PATH_INFO"] = uri.getPathInfo();
    env_["SERVER_NAME"] = host;
    env_["SERVER_PORT"] = std::to_string(port);
    env_["HTTP_HOST"] = host_port;
    env_["REMOTE_ADDR"] = request.getClient().getClientAddress();
    env_["REDIRECT_STATUS"] = "200";
    env_["SERVER_SOFTWARE"] = "Webserv/1.2";
    env_["REQUEST_SCHEME"] = "HTTP";
    env_["HTTP_VERSION"] = "1.1";
    env_["UPLOAD_TMP_DIR"] = uri.getConfig()->get<std::string>("cgi_tmp_dir").value_or(root);
    env_["TMP_DIR"] = env_["UPLOAD_TMP_DIR"];
    env_["CONTENT_TYPE"] = request.getHeaders().getContentType().value_or("");
    env_["CONTENT_LENGTH"] = std::to_string(request.getHeaders().getContentLength().value_or(0));

    if (uri.getConfig()->get<bool>("42_tester").value_or(false))
    {
        env_["PATH_INFO"] = request.getTarget(); // ! This is only correct (but necessary) for the tester;
    }

    // Add HTTP_ headers
    const HttpHeaders &headers = request.getHeaders();

    // Map common request headers to CGI environment variables
    addHttpHeaderToEnv("Cookie", headers, "; ");
    addHttpHeaderToEnv("User-Agent", headers);
    addHttpHeaderToEnv("Accept", headers);
    addHttpHeaderToEnv("Accept-Language", headers);
    addHttpHeaderToEnv("Accept-Encoding", headers);

    setXHeaders(headers);
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

void CgiEnvironment::setXHeaders(const HttpHeaders &headers)
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
        std::ranges::transform(key.begin(), key.end(), key.begin(), ::toupper);
        std::ranges::replace(key.begin(), key.end(), '-', '_');

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