#include "webserv/config/AConfig.hpp"
#include <webserv/utils/AutoIndex.hpp>
#include <webserv/utils/FileUtils.hpp>
#include <webserv/handler/URI.hpp>

#include <format>
#include <string>
#include <vector>

std::string AutoIndex::generate(const std::string &dir, const URI &uri)
{
    std::ostringstream html;

    html << "<!DOCTYPE html>\n"
         << "<html><head>\n"
         << "<title>Index of " << dir << "</title>\n"
         << "<style>\n"
         << "body { font-family: Arial, sans-serif; margin: 40px; }\n"
         << "h1 { border-bottom: 1px solid #ccc; }\n"
         << "table { border-collapse: collapse; width: 100%; }\n"
         << "th, td { text-align: left; padding: 8px 12px; border-bottom: 1px solid #ddd; }\n"
         << "th { background-color: #f5f5f5; }\n"
         << "a { text-decoration: none; color: #0066cc; }\n"
         << "a:hover { text-decoration: underline; }\n"
         << ".dir { font-weight: bold; }\n"
         << "</style>\n"
         << "</head><body>\n"
         << "<h1>Index of " << dir << "</h1>\n"
         << "<table>\n"
         << "<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";

    // Add parent directory link if not root
    if (dir != "/")
    {
        html << "<tr><td><a href=\"../\" class=\"dir\">[Parent Directory]</a></td><td>-</td><td>-</td></tr>\n";
    }

    auto entries = FileUtils::listDirectory(dir);

    for (const auto &entry : entries)
    {
        std::string href = uri.getUriForPath(entry.path().string());
        if (entry.is_directory())
        {
            href += "/";
        }

        html << "<tr><td><a href=\"" << href << "\"";
        if (entry.is_directory())
        {
            html << " class=\"dir\"";
        }
        html << ">" << entry.path().filename().string();
        if (entry.is_directory())
        {
            html << "/";
        }
        html << "</a></td>" << "<td></td>"; //<< std::format("{:%Y-%m-%d %H:%M:%S}", entry.last_write_time()) << "</td>"
        html << "<td>" << (entry.is_directory() ? "-" : formatFileSize(entry.file_size())) << "</td></tr>\n";
    }

    html << "</table>\n"
         << "<hr><p><em>webserv</em></p>\n"
         << "</body></html>\n";

    return html.str();
}

std::string AutoIndex::formatFileSize(uintmax_t size)
{
    static const std::vector<std::string> units = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    auto displaySize = static_cast<double>(size);

    while (displaySize >= 1024 && unitIndex < 4)
    {
        displaySize /= 1024;
        ++unitIndex;
    }

    std::ostringstream oss;
    oss.precision(2);
    oss << std::fixed << displaySize << " " << units.at(unitIndex);
    return oss.str();
}