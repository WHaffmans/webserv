#include <webserv/config/directive/DirectiveValue.hpp>

#include <cstddef> // for size_t
#include <string>  // for string, basic_string, allocator, operator+, char_traits, to_string, operator<<
#include <utility> // for pair
#include <variant> // for visit
#include <vector>  // for vector

DirectiveValue::DirectiveValue(DirectiveValueType value) : value_(std::move(value)) {}

std::string DirectiveValue::toString() const
{
    return std::visit(overloaded{[](int val) { return std::to_string(val); },
                                 [](size_t val) { return std::to_string(val); },
                                 [](bool val) { return val ? std::string("true") : std::string("false"); },
                                 [](const std::string &val) { return val; },
                                 [](const std::vector<std::string> &val) {
                                     std::string result = "[";
                                     for (size_t i = 0; i < val.size(); ++i)
                                     {
                                         if (i > 0)
                                         {
                                             result += ", ";
                                         }
                                         result += "\"" + val[i] + "\"";
                                     }
                                     result += "]";
                                     return result;
                                 },
                                 [](const std::pair<int, std::string> &val) {
                                     return std::to_string(val.first) + " \"" + val.second + "\"";
                                 }},
                      value_);
}

std::ostream &operator<<(std::ostream &os, const DirectiveValue &dv)
{
    return os << dv.toString();
}