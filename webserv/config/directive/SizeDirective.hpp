#pragma once

#include "ADirective.hpp"

#include <any>

class SizeDirective : public ADirective
{
  public:
    SizeDirective() = delete;

    SizeDirective(const std::string &name, const std::string &value) : ADirective(name) { parse(value); }

    SizeDirective(const SizeDirective &other) = delete;
    SizeDirective &operator=(const SizeDirective &other) = delete;
    SizeDirective(SizeDirective &&other) noexcept = delete;
    SizeDirective &operator=(SizeDirective &&other) noexcept = delete;

    ~SizeDirective() override = default;

    void parse(const std::string &value) override;

    [[nodiscard]] DirectiveValueType getValue() const override;

  private:
    size_t value_ = 0;
};