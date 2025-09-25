#pragma once

#include <webserv/config/directive/DirectiveValue.hpp>
#include <iostream>
#include <string>



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

    [[nodiscard]] virtual DirectiveValueType getValue() const = 0;
    [[nodiscard]] DirectiveValue get() const;
    [[nodiscard]] std::string getName() const;
    // [[nodiscard]] std::string toString() const;

  protected:
    std::string name_;
};

// Non-member stream operator for ADirective
std::ostream &operator<<(std::ostream &os, const ADirective &directive);