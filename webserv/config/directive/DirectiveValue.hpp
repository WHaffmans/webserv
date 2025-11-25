#pragma once

#include <cstddef>  // for size_t
#include <iostream> // for ostream, operator<<
#include <optional> // for optional, nullopt
#include <string>   // for string, basic_string, char_traits, to_string
#include <utility>  // for pair, move
#include <variant>  // for variant, visit, get, holds_alternative
#include <vector>   // for vector

// Visitor overload pattern for std::visit
template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};

template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// Define all possible directive value types
using DirectiveValueType = std::variant<int, // listen, error_page status, cgi_timeout
                                        size_t,
                                        bool,                       // autoindex
                                        std::string,                // host, server_name, root, cgi_pass, upload_store
                                        std::vector<std::string>,   // index, allowed_methods, cgi_handler
                                        std::pair<int, std::string> // error_page (status, path), redirect
                                        >;

class DirectiveValue
{
  public:
    DirectiveValue(DirectiveValueType value);

    template <typename T> T get() const { return std::get<T>(value_); }

    template <typename T> std::optional<T> try_get() const
    {
        if (std::holds_alternative<T>(value_))
        {
            return std::get<T>(value_);
        }
        return std::nullopt;
    }

    template <typename T> [[nodiscard]] bool holds() const noexcept { return std::holds_alternative<T>(value_); }

    [[nodiscard]] std::string toString() const;

  private:
    DirectiveValueType value_;
};

// Non-member stream operator for DirectiveValue
std::ostream &operator<<(std::ostream &os, const DirectiveValue &dv);
