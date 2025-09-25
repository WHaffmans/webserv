#include <webserv/config/directive/IntDirective.hpp> // for IntDirective

#include <any>

void IntDirective::parse(const std::string &value)
{
    value_ = std::stoi(value); // TODO: check parsing
}

std::any IntDirective::getValue() const
{
    return value_;
}