#include "webserv/config/directive/ADirective.hpp"

#include <functional>
#include <memory>
#include <string_view>
#include <unordered_map>

class DirectiveFactory
{
  public:
    static std::unique_ptr<ADirective> createDirective(const std::string &line);

  private:
    using CreatorFunc = std::function<std::unique_ptr<ADirective>(const std::string &, const std::string &arg)>;

    static const std::unordered_map<std::string_view, CreatorFunc> &getFactories();
    static std::unique_ptr<ADirective> create(std::string_view type, const std::string &name, const std::string &arg);

    struct DirectiveInfo
    {
        std::string_view name;
        std::string_view type;
        std::string_view context;
    };

    constexpr static std::array<DirectiveInfo, 15> supportedDirectives = {{
        {.name = "listen", .type = "IntDirective", .context = "SL"},
        {.name = "host", .type = "StringDirective", .context = "SL"},
        {.name = "server_name", .type = "VectorDirective", .context = "SL"},
        {.name = "root", .type = "StringDirective", .context = "SL"},
        {.name = "index", .type = "VectorDirective", .context = "SL"},
        {.name = "error_page", .type = "IntStringDirective", .context = "SL"},
        {.name = "client_max_body_size", .type = "SizeDirective", .context = "SL"},
        {.name = "autoindex", .type = "BoolDirective", .context = "L"},
        {.name = "allowed_methods", .type = "VectorDirective", .context = "L"},
        {.name = "cgi_pass", .type = "StringDirective", .context = "L"},
        {.name = "cgi_ext", .type = "VectorDirective", .context = "L"},
        {.name = "cgi_timeout", .type = "IntDirective", .context = "L"},
        {.name = "upload_enabled", .type = "BoolDirective", .context = "L"},
        {.name = "upload_store", .type = "StringDirective", .context = "L"},
        {.name = "redirect", .type = "VectorDirective", .context = "L"},
    }};
};