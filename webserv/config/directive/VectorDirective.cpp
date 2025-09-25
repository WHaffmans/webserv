#include <webserv/config/directive/VectorDirective.hpp> // for IntDirective

#include <any>
#include <sstream> // for std::getline, std::basic_istream, std::char_traits, std::basic_stringbuf

void VectorDirective::parse(const std::string &value)
{
    std::stringstream ss(value);
    while(ss.good())
    {
        std::string item;
        std::getline(ss, item, ' ');// index    indx.html
        if (!item.empty())
        {
            value_.push_back(item);
        }
    }
}

std::any VectorDirective::getValue() const
{
    return value_;
}