#pragma once

#include <webserv/config/directive/ADirective.hpp> // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp>

#include <memory> // for unique_ptr
#include <optional>
#include <string> // for string
#include <vector> // for vector

class AConfig
{
  public:
    AConfig() = default;
    AConfig(const AConfig *parent);
    AConfig(const AConfig &other) = delete;
    AConfig &operator=(const AConfig &other) = delete;
    AConfig(AConfig &&other) noexcept = delete;
    AConfig &operator=(AConfig &&other) noexcept = delete;

    virtual ~AConfig() = default;

    void addDirective(const std::string &line);
    [[nodiscard]] std::string getErrorPage(int statusCode) const;

    [[nodiscard]] bool hasDirective(const std::string &name) const;

    template <typename T>
    std::optional<T> get(const std::string &name) const
    {
        const auto *directive = getDirective(name);
        if (!directive)
        {
            return std::nullopt;
        }

        auto value = directive->getValue();
        if (value.holds<T>())
        {
            return value.get<T>();
        }
        return std::nullopt;
    }

    protected:
    [[nodiscard]] const ADirective *getDirective(const std::string &name) const;
    virtual void parseBlock(const std::string &block) = 0;
    void parseDirectives(const std::string &declarations);
    std::vector<std::unique_ptr<ADirective>>
        directives_;                  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
    const AConfig *parent_ = nullptr; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};