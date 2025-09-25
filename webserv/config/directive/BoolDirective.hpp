#pragma once

#include "ADirective.hpp"

#include <any>

class BoolDirective : public ADirective
{
    public:
        BoolDirective() = delete;

        BoolDirective(const std::string &name, const std::string &value) : ADirective(name) { parse(value); }

        BoolDirective(const BoolDirective &other) = delete;
        BoolDirective &operator=(const BoolDirective &other) = delete;
        BoolDirective(BoolDirective &&other) noexcept = delete;
        BoolDirective &operator=(BoolDirective &&other) noexcept = delete;

        ~BoolDirective() override = default;

        void parse(const std::string &value) override;

        [[nodiscard]] std::any getValue() const override;

    private:
        bool value_ = false;
};