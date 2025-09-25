#pragma once

#include "ADirective.hpp"

class StringDirective : public ADirective
{
  public:
    StringDirective() = delete;

    StringDirective(const std::string &name, const std::string &value) : ADirective(name) { parse(value); }

    StringDirective(const StringDirective &other) = delete;
    StringDirective &operator=(const StringDirective &other) = delete;
    StringDirective(StringDirective &&other) noexcept = delete;
    StringDirective &operator=(StringDirective &&other) noexcept = delete;

    ~StringDirective() override = default;

    void parse(const std::string &value) override;

    [[nodiscard]] DirectiveValueType getValue() const override;

  private:
    std::string value_;
};