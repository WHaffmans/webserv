#pragma once

#include <string_view>
#include <cstdint>

namespace Http {
    // HTTP Methods
    namespace Method {
        constexpr std::string_view GET = "GET";
        constexpr std::string_view POST = "POST";
        constexpr std::string_view PUT = "PUT";
        constexpr std::string_view DELETE = "DELETE";
        constexpr std::string_view HEAD = "HEAD";
        constexpr std::string_view OPTIONS = "OPTIONS";
        constexpr std::string_view PATCH = "PATCH";
    }

    // HTTP Versions
    namespace Version {
        constexpr std::string_view HTTP_1_0 = "HTTP/1.0";
        constexpr std::string_view HTTP_1_1 = "HTTP/1.1";
        constexpr std::string_view HTTP_2_0 = "HTTP/2.0";
    }

    // Status Codes
    namespace StatusCode {
        constexpr uint16_t OK = 200;
        constexpr uint16_t CREATED = 201;
        constexpr uint16_t NO_CONTENT = 204;
        constexpr uint16_t BAD_REQUEST = 400;
        constexpr uint16_t UNAUTHORIZED = 401;
        constexpr uint16_t FORBIDDEN = 403;
        constexpr uint16_t NOT_FOUND = 404;
        constexpr uint16_t METHOD_NOT_ALLOWED = 405;
        constexpr uint16_t INTERNAL_SERVER_ERROR = 500;
        constexpr uint16_t NOT_IMPLEMENTED = 501;
        constexpr uint16_t BAD_GATEWAY = 502;
        constexpr uint16_t SERVICE_UNAVAILABLE = 503;
    }

    // Header Names
    namespace Header {
        constexpr std::string_view CONTENT_TYPE = "Content-Type";
        constexpr std::string_view CONTENT_LENGTH = "Content-Length";
        constexpr std::string_view HOST = "Host";
        constexpr std::string_view USER_AGENT = "User-Agent";
        constexpr std::string_view ACCEPT = "Accept";
        constexpr std::string_view AUTHORIZATION = "Authorization";
        constexpr std::string_view CACHE_CONTROL = "Cache-Control";
        constexpr std::string_view CONNECTION = "Connection";
        constexpr std::string_view TRANSFER_ENCODING = "Transfer-Encoding";
    }

    // MIME Types
    namespace MimeType {
        constexpr std::string_view TEXT_HTML = "text/html";
        constexpr std::string_view TEXT_PLAIN = "text/plain";
        constexpr std::string_view APPLICATION_JSON = "application/json";
        constexpr std::string_view APPLICATION_XML = "application/xml";
        constexpr std::string_view IMAGE_JPEG = "image/jpeg";
        constexpr std::string_view IMAGE_PNG = "image/png";
        constexpr std::string_view OCTET_STREAM = "application/octet-stream";
    }

    // Protocol Constants
    namespace Protocol {
        constexpr std::string_view CRLF = "\r\n";
        constexpr std::string_view HEADER_SEPARATOR = ": ";
        constexpr std::string_view SPACE = " ";
        constexpr size_t MAX_HEADER_SIZE = 8192;
        constexpr size_t MAX_BODY_SIZE = 1024 * 1024; // 1MB
        constexpr size_t MAX_URI_LENGTH = 2048;
    }
}