#include <webserv/config/directive/VectorDirective.hpp> // for IntDirective

#include <sstream> // for basic_stringstream, stringstream

VectorDirective::VectorDirective(const std::string &name, const std::string &value)
    : ADirective(name) // NOLINT(bugprone-easily-swappable-parameters)
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
            value_.push_back(item);
        }
    }
}

DirectiveValueType VectorDirective::getValueType() const
{
    return value_;
}