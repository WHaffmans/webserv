#include "webserv/handler/CgiEnvironment.hpp"

#include "webserv/log/Log.hpp"

#include <cstring> // for strcpy

CgiEnvironment::CgiEnvironment(const URI &uri, const HttpRequest &request)
{
    env_["GATEWAY_INTERFACE"] = "CGI/1.1";
    env_["SERVER_PROTOCOL"] = "HTTP/1.1";
    env_["REQUEST_METHOD"] = request.getMethod();
    env_["SCRIPT_NAME"] = uri.getBaseName();
    env_["SCRIPT_FILENAME"] = uri.getFullPath(); // Full filesystem path to the script (required by PHP)
    env_["QUERY_STRING"] = uri.getQuery();
    env_["REQUEST_URI"] = request.getTarget();
    env_["PATH_INFO"] = uri.getPathInfo();

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
    env_["REMOTE_ADDR"] = "<REMOTE_ADDR>"; // Placeholder, should be set to actual remote address
    env_["REDIRECT_STATUS"] = "200";       // Required by PHP with force-cgi-redirect enabled
    env_["SERVER_SOFTWARE"] = "Webserv/1.0";
    env_["REQUEST_SCHEME"] = "HTTP";
    env_["HTTP_VERSION"] = "1.1";
    // Add HTTP_ headers
    const HttpHeaders &headers = request.getHeaders();
    env_["HTTP_COOKIE"] = headers.get("Cookie");
    env_["HTTP_USER_AGENT"] = headers.get("User-Agent");
    env_["HTTP_ACCEPT"] = headers.get("Accept");
    env_["HTTP_ACCEPT_LANGUAGE"] = headers.get("Accept-Language");
    env_["HTTP_ACCEPT_ENCODING"] = headers.get("Accept-Encoding");
        

}

char **CgiEnvironment::toEnvp() const
{
    char **envp = new char *[env_.size() + 1];
    size_t index = 0;
    for (std::map<std::string, std::string>::const_iterator it = env_.begin(); it != env_.end(); ++it, ++index)
    {
        std::string entry = it->first + "=" + it->second;
        envp[index] = new char[entry.size() + 1];
        std::strcpy(envp[index], entry.c_str());
    }
    envp[index] = nullptr; // Null-terminate the array
    return envp;
}