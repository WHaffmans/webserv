
#include <webserv/handler/MIMETypes.hpp>

#include <webserv/http/HttpConstants.hpp>

#include <utility> // for pair

MIMETypes::MIMETypes()
{
    initializeDefaults();
}

std::string MIMETypes::getType(const std::string &extension) const
{
    auto it = mimeMap.find(extension);
    if (it != mimeMap.end())
    {
        return it->second;
    }
    return "application/octet-stream"; // Default MIME type
}

void MIMETypes::setType(const std::string &extension, const std::string &mimeType)
{
    mimeMap[extension] = mimeType;
}

void MIMETypes::initializeDefaults()
{
    mimeMap["html"] = Http::MimeType::TEXT_HTML;
    mimeMap["htm"] = Http::MimeType::TEXT_HTML;
    mimeMap["css"] = Http::MimeType::TEXT_CSS;
    mimeMap["js"] = Http::MimeType::APPLICATION_JAVASCRIPT;
    mimeMap["json"] = Http::MimeType::APPLICATION_JSON;
    mimeMap["png"] = Http::MimeType::IMAGE_PNG;
    mimeMap["jpg"] = Http::MimeType::IMAGE_JPEG;
    mimeMap["jpeg"] = Http::MimeType::IMAGE_JPEG;
    mimeMap["gif"] = Http::MimeType::IMAGE_GIF;
    mimeMap["svg"] = Http::MimeType::IMAGE_SVG;
    mimeMap["txt"] = Http::MimeType::TEXT_PLAIN;
    mimeMap["xml"] = Http::MimeType::APPLICATION_XML;
    mimeMap["pdf"] = Http::MimeType::APPLICATION_PDF;
}