#include <webserv/config/directive/VectorDirective.hpp> // for IntDirective

#include <webserv/config/directive/ADirective.hpp>      // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp>  // for DirectiveValueType

#include <sstream> // for basic_stringstream, stringstream

#include <linux/limits.h>

VectorDirective::VectorDirective(const std::string &name, const std::string &value) : ADirective(name)
{
    parse(value);
}

void VectorDirective::parse(const std::string &value)
{
    std::stringstream ss(value);
    while (ss.good())
    {
        std::string item;
        std::getline(ss, item, ' '); // index    indx.html
        if (!item.empty())
        {
            if (item.size() > PATH_MAX)
            {
                throw std::invalid_argument("VectorDirective: string value exceeds maximum length");
            }
            value_.push_back(item);
        }
    }
}

DirectiveValueType VectorDirective::getValueType() const
{
    return value_;
}