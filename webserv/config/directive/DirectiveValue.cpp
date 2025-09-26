#include <webserv/config/directive/ADirective.hpp> // for ADirective, operator<<
#include <webserv/config/directive/DirectiveValue.hpp>

#include <cstddef> // for size_t
#include <string>  // for string, basic_string, allocator, char_traits, operator+, to_string, operator<<
#include <utility> // for pair
#include <variant> // for visit
#include <vector>  // for vector

//

// std::string DirectiveValue::toString() const
// {
//     return std::visit(
//         [](const auto &val) -> std::string {
//             using T = std::decay_t<decltype(val)>;

//             if constexpr (std::is_same_v<T, int> || std::is_same_v<T, size_t>)
//             {
//                 return std::to_string(val);
//             }
//             else if constexpr (std::is_same_v<T, std::string>)
//             {
//                 return val;
//             }
//             else if constexpr (std::is_same_v<T, bool>)
//             {
//                 return val ? "true" : "false";
//             }
//             else if constexpr (std::is_same_v<T, std::vector<std::string>>)
//             {
//                 std::string result = "[";
//                 for (size_t i = 0; i < val.size(); ++i)
//                 {
//                     if (i > 0)
//                     {
//                         result += ", ";
//                     }
//                     result += "\"" + val[i] + "\"";
//                 }
//                 result += "]";
//                 return result;
//             }
//             else if constexpr (std::is_same_v<T, std::pair<int, std::string>>)
//             {
//                 return std::to_string(val.first) + " \"" + val.second + "\"";
//             }
//             else
//             {
//                 return "<unknown>";
//             }
//         },
//         value_);
// }

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