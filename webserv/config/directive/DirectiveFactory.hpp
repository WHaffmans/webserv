#pragma once
#include <webserv/config/directive/ADirective.hpp> // for ADirective

#include <array>         // for array
#include <functional>    // for function
#include <memory>        // for unique_ptr
#include <string>        // for string
#include <string_view>   // for string_view
#include <unordered_map> // for unordered_map

class DirectiveFactory
{
  public:
    static std::unique_ptr<ADirective> createDirective(const std::string &line);

    struct DirectiveInfo
    {
        std::string_view name;
        std::string_view type;
        std::string_view context;
    };


    constexpr static std::array<DirectiveInfo, 15> supportedDirectives = {{
        {.name = "listen", .type = "IntDirective", .context = "S"},
        {.name = "host", .type = "StringDirective", .context = "S"},
        {.name = "server_name", .type = "StringDirective", .context = "S"},
        {.name = "root", .type = "StringDirective", .context = "Sl"},
        {.name = "index", .type = "VectorDirective", .context = "sl"},
        {.name = "error_page", .type = "IntStringDirective", .context = "gsl"},
        {.name = "client_max_body_size", .type = "SizeDirective", .context = "gsl"},
        {.name = "autoindex", .type = "BoolDirective", .context = "gsl"},
        {.name = "allowed_methods", .type = "VectorDirective", .context = "gsl"},
        {.name = "cgi_enabled", .type = "BoolDirective", .context = "gsl"},
        {.name = "cgi_ext", .type = "VectorDirective", .context = "gsl"},
        {.name = "cgi_timeout", .type = "IntDirective", .context = "gsl"},
        {.name = "upload_enabled", .type = "BoolDirective", .context = "gsl"},
        {.name = "upload_store", .type = "StringDirective", .context = "gsl"},
        {.name = "redirect", .type = "VectorDirective", .context = "l"},
    }};
  private:
    using CreatorFunc = std::function<std::unique_ptr<ADirective>(const std::string &, const std::string &arg)>;

    static const std::unordered_map<std::string_view, CreatorFunc> &getFactories();
    static std::unique_ptr<ADirective> create(std::string_view type, const std::string &name, const std::string &arg);




};