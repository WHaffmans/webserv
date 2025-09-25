#include <webserv/config/directive/ADirective.hpp> // for ADirective

DirectiveValue ADirective::get() const
{
    return {getValue()};
}

std::string ADirective::getName() const
{
    return name_;
}

void ADirective::setName(const std::string &name)
{
    name_ = name;
}