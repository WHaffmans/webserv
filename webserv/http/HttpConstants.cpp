#include "HttpConstants.hpp"

#include <string>

namespace Http
{
std::string getStatusCodeReason(uint16_t statusCode)
{
    for (const auto &info : statusCodeInfos)
    {
        if (info.code == statusCode)
        {
            return std::string(info.reason);
        }
    }
    return "Unknown Status";
}
} // namespace Http