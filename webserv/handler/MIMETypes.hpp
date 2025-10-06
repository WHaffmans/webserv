#ifndef MIMETYPES_HPP
#define MIMETYPES_HPP

#include <map>
#include <string>

class MIMETypes
{
  public:
    MIMETypes();

    // Returns the MIME type for a given file extension (e.g., "html" -> "text/html")
    [[nodiscard]] std::string getType(const std::string &extension) const;

    // Adds or updates a MIME type mapping
    void setType(const std::string &extension, const std::string &mimeType);

  private:
    std::map<std::string, std::string> mimeMap;

    void initializeDefaults();
};

#endif // MIMETYPES_HPP