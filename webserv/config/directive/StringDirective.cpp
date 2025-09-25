#include <webserv/config/directive/StringDirective.hpp> // for IntDirective

#include <any>

void StringDirective::parse(const std::string &value)
{
    value_ = value;
}

DirectiveValueType StringDirective::getValue() const
{
    return value_;
}