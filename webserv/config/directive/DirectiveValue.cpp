#include <webserv/config/directive/ADirective.hpp>
#include <webserv/config/directive/DirectiveValue.hpp>

#include <cstddef>
#include <string>
#include <type_traits> // for is_same_v, decay_t
#include <utility>     // for pair, move
#include <variant>     // for variant, visit
#include <vector>      // for vector
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

std::ostream &operator<<(std::ostream &os, const ADirective &directive)
{
    return os << directive.getName() << ": " << directive.get().toString();
}