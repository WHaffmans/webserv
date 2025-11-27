#pragma once

#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValue, DirectiveValueType

#include <iostream> // for ostream
#include <string>   // for string, basic_string

class ADirective
{
  public:
    ADirective() = delete;

    ADirective(std::string name);

    ADirective(const ADirective &other) = delete;
    ADirective &operator=(const ADirective &other) = delete;
    ADirective(ADirective &&other) noexcept = delete;
    ADirective &operator=(ADirective &&other) noexcept = delete;

    virtual ~ADirective() = default;

    void setName(const std::string &name);
    virtual void parse(const std::string &value) = 0;

    [[nodiscard]] virtual DirectiveValueType getValueType() const = 0;
    [[nodiscard]] DirectiveValue getValue() const;
    [[nodiscard]] std::string getName() const;

  protected:
    std::string name_; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};

// Non-member stream operator for ADirective
std::ostream &operator<<(std::ostream &os, const ADirective &directive);