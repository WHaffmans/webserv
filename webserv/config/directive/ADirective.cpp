#include <webserv/config/directive/ADirective.hpp>
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValue, operator<<

DirectiveValue ADirective::getValue() const
{
    return {getValueType()};
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

std::ostream &operator<<(std::ostream &os, const ADirective &directive)
{
    return os << directive.getName() << ": " << directive.getValue().toString();
}