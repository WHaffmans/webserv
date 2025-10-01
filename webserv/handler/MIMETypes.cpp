
#include <webserv/handler/MIMETypes.hpp>



MIMETypes::MIMETypes() {
	initializeDefaults();
}

std::string MIMETypes::getType(const std::string& extension) const {
	auto it = mimeMap.find(extension);
	if (it != mimeMap.end()) {
		return it->second;
	}
	return "application/octet-stream"; // Default MIME type
}

void MIMETypes::setType(const std::string& extension, const std::string& mimeType) {
	mimeMap[extension] = mimeType;
}

void MIMETypes::initializeDefaults() {
	mimeMap["html"] = "text/html";
	mimeMap["htm"] = "text/html";
	mimeMap["css"] = "text/css";
	mimeMap["js"] = "application/javascript";
	mimeMap["json"] = "application/json";
	mimeMap["png"] = "image/png";
	mimeMap["jpg"] = "image/jpeg";
	mimeMap["jpeg"] = "image/jpeg";
	mimeMap["gif"] = "image/gif";
	mimeMap["svg"] = "image/svg+xml";
	mimeMap["txt"] = "text/plain";
	mimeMap["xml"] = "application/xml";
	mimeMap["pdf"] = "application/pdf";
	// Add more default MIME types as needed
}