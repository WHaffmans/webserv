#pragma once

#include "ADirective.hpp"

#include <any>

class IntDirective : public ADirective
{
  public:
    IntDirective() = delete;

    IntDirective(const std::string &name, const std::string &value) : ADirective(name) { parse(value); }

    IntDirective(const IntDirective &other) = delete;
    IntDirective &operator=(const IntDirective &other) = delete;
    IntDirective(IntDirective &&other) noexcept = delete;
    IntDirective &operator=(IntDirective &&other) noexcept = delete;

    ~IntDirective() override = default;

    void parse(const std::string &value) override;

    [[nodiscard]] std::any getValue() const override;

  private:
    int value_ = 0;
};