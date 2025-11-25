#pragma once

#include <array>       // for array
#include <cstdint>     // for uint16_t
#include <string>      // for string
#include <string_view> // for string_view

#include <stddef.h> // for size_t

namespace Http
{
// HTTP Methods
namespace Method
{
constexpr std::string_view GET = "GET";
constexpr std::string_view POST = "POST";
constexpr std::string_view PUT = "PUT";
constexpr std::string_view DELETE = "DELETE";
constexpr std::string_view HEAD = "HEAD";
constexpr std::string_view OPTIONS = "OPTIONS";
constexpr std::string_view PATCH = "PATCH";
} // namespace Method

// HTTP Versions
namespace Version
{
constexpr std::string_view HTTP_1_0 = "HTTP/1.0";
constexpr std::string_view HTTP_1_1 = "HTTP/1.1";
constexpr std::string_view HTTP_2_0 = "HTTP/2.0";
} // namespace Version

// Status Codes
namespace StatusCode
{
constexpr uint16_t OK = 200;
constexpr uint16_t CREATED = 201;
constexpr uint16_t NO_CONTENT = 204;

constexpr uint16_t MOVED_PERMANENTLY = 301;
constexpr uint16_t FOUND = 302;
constexpr uint16_t SEE_OTHER = 303;
constexpr uint16_t TEMPORARY_REDIRECT = 307;
constexpr uint16_t PERMANENT_REDIRECT = 308;

constexpr uint16_t BAD_REQUEST = 400;
constexpr uint16_t UNAUTHORIZED = 401;
constexpr uint16_t FORBIDDEN = 403;
constexpr uint16_t NOT_FOUND = 404;
constexpr uint16_t METHOD_NOT_ALLOWED = 405;
constexpr uint16_t PAYLOAD_TOO_LARGE = 413;
constexpr uint16_t URI_TOO_LONG = 414;
constexpr uint16_t INTERNAL_SERVER_ERROR = 500;
constexpr uint16_t NOT_IMPLEMENTED = 501;
constexpr uint16_t BAD_GATEWAY = 502;
constexpr uint16_t SERVICE_UNAVAILABLE = 503;
constexpr uint16_t GATEWAY_TIMEOUT = 504;
} // namespace StatusCode

struct StatusCodeInfo
{
    uint16_t code;
    std::string_view reason;
};

static constexpr std::array<StatusCodeInfo, 19> statusCodeInfos
    = {{{.code = StatusCode::OK, .reason = "OK"},
        {.code = StatusCode::CREATED, .reason = "Created"},
        {.code = StatusCode::NO_CONTENT, .reason = "No Content"},
        {.code = StatusCode::MOVED_PERMANENTLY, .reason = "Moved Permanently"},
        {.code = StatusCode::FOUND, .reason = "Found"},
        {.code = StatusCode::SEE_OTHER, .reason = "See Other"},
        {.code = StatusCode::TEMPORARY_REDIRECT, .reason = "Temporary Redirect"},
        {.code = StatusCode::PERMANENT_REDIRECT, .reason = "Permanent Redirect"},
        {.code = StatusCode::BAD_REQUEST, .reason = "Bad Request"},
        {.code = StatusCode::UNAUTHORIZED, .reason = "Unauthorized"},
        {.code = StatusCode::FORBIDDEN, .reason = "Forbidden"},
        {.code = StatusCode::NOT_FOUND, .reason = "Not Found"},
        {.code = StatusCode::METHOD_NOT_ALLOWED, .reason = "Method Not Allowed"},
        {.code = StatusCode::PAYLOAD_TOO_LARGE, .reason = "Payload Too Large"},
        {.code = StatusCode::URI_TOO_LONG, .reason = "URI Too Long"},
        {.code = StatusCode::INTERNAL_SERVER_ERROR, .reason = "Internal Server Error"},
        {.code = StatusCode::NOT_IMPLEMENTED, .reason = "Not Implemented"},
        {.code = StatusCode::BAD_GATEWAY, .reason = "Bad Gateway"},
        {.code = StatusCode::SERVICE_UNAVAILABLE, .reason = "Service Unavailable"}}};

std::string getStatusCodeReason(uint16_t statusCode) noexcept;

// Header Names
namespace Header
{
constexpr std::string_view CONTENT_TYPE = "Content-Type";
constexpr std::string_view CONTENT_LENGTH = "Content-Length";
constexpr std::string_view REDIRECT_LOCATION = "Location";
constexpr std::string_view HOST = "Host";
constexpr std::string_view USER_AGENT = "User-Agent";
constexpr std::string_view ACCEPT = "Accept";
constexpr std::string_view AUTHORIZATION = "Authorization";
constexpr std::string_view CACHE_CONTROL = "Cache-Control";
constexpr std::string_view CONNECTION = "Connection";
constexpr std::string_view TRANSFER_ENCODING = "Transfer-Encoding";
} // namespace Header

// MIME Types
namespace MimeType
{
constexpr std::string_view TEXT_HTML = "text/html";
constexpr std::string_view TEXT_PLAIN = "text/plain";
constexpr std::string_view APPLICATION_JSON = "application/json";
constexpr std::string_view APPLICATION_XML = "application/xml";
constexpr std::string_view IMAGE_JPEG = "image/jpeg";
constexpr std::string_view IMAGE_PNG = "image/png";
constexpr std::string_view OCTET_STREAM = "application/octet-stream";
} // namespace MimeType

// Protocol Constants
namespace Protocol
{
constexpr std::string_view CRLF = "\r\n";
constexpr std::string_view DOUBLE_CRLF = "\r\n\r\n";
constexpr std::string_view HEADER_SEPARATOR = ": ";
constexpr std::string_view SPACE = " ";
constexpr size_t MAX_HEADER_SIZE = 8192;
constexpr size_t MAX_HEADER_COUNT = 64;
constexpr size_t MAX_BODY_SIZE = size_t(1024) * size_t(1024); // 1MB
constexpr size_t MAX_URI_LENGTH = 2048;
constexpr std::string_view HTTP_SCHEME = "http";
} // namespace Protocol
} // namespace Http