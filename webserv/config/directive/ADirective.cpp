#include <webserv/config/directive/ADirective.hpp>

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

// Non-member stream operator implementations
std::ostream &operator<<(std::ostream &os, const DirectiveValue &dv)
{
    return os << dv.toString();
}

