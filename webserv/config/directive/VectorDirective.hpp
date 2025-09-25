#pragma once

#include "ADirective.hpp"

#include <string>
#include <vector>

class VectorDirective : public ADirective
{
  public:
    VectorDirective() = delete;

    VectorDirective(const std::string &name, const std::string &value) : ADirective(name) { parse(value); }

    VectorDirective(const VectorDirective &other) = delete;
    VectorDirective &operator=(const VectorDirective &other) = delete;
    VectorDirective(VectorDirective &&other) noexcept = delete;
    VectorDirective &operator=(VectorDirective &&other) noexcept = delete;

    ~VectorDirective() override = default;

    void parse(const std::string &value) override;

    [[nodiscard]] DirectiveValueType getValue() const override;

  private:
    std::vector<std::string> value_;
};