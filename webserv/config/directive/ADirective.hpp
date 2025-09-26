#pragma once

#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValue, DirectiveValueType

#include <iostream> // for ostream
#include <string>   // for string, basic_string
#include <utility>  // for move

class ADirective
{
  public:
    ADirective() = delete;

    ADirective(std::string name) : name_(std::move(name)) {}

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

    // [[nodiscard]] std::string toString() const;
    template <typename T> [[nodiscard]] T getValueAs() const
    {
        if (getValue().holds<T>())
        {
            return getValue().get<T>();
        }
        return T(); //TODO: does this work for all types?
    }

  protected:
    std::string name_;
};

// Non-member stream operator for ADirective
std::ostream &operator<<(std::ostream &os, const ADirective &directive);