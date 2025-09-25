#include <webserv/config/directive/SizeDirective.hpp> // for SizeDirective
#include <webserv/config/utils.hpp>                   // for trim

#include <algorithm>
#include <any>
#include <cctype>
#include <stdexcept>

void SizeDirective::parse(const std::string &value)
{
    size_t multiplier = 1;
    size_t idx = 0;
    std::string number = value;
    std::ranges::transform(number, number.begin(), ::tolower);
    number = utils::trim(number);
    if (number.find_first_not_of("01234567890kmg") != std::string::npos)
    {
        throw std::invalid_argument("Invalid size directive: " + value);
    }
    value_ = std::stoul(number, &idx);
    if (idx == number.size())
    {
        return;
    }
    std::string suffix = number.substr(idx);
    if (suffix == "k")
    {
        multiplier = 1024;
    }
    else if (suffix == "m")
    {
        multiplier = 1024 * 1024;
    }
    else if (suffix == "g")
    {
        multiplier = 1024 * 1024 * 1024;
    }
    else
    {
        throw std::invalid_argument("Invalid size directive: " + value);
    }

    value_ *= multiplier;
}

std::any SizeDirective::getValue() const
{
    return value_;
}