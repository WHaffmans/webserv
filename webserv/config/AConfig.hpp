#pragma once

#include <webserv/config/directive/ADirective.hpp> // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp>

#include <memory>   // for unique_ptr
#include <optional> // for nullopt, optional
#include <string>   // for string
#include <vector>   // for vector

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
    // [[nodiscard]] virtual std::string getName() const = 0;
    [[nodiscard]] virtual std::string getType() const = 0;
    void addDirective(const std::string &line);
    [[nodiscard]] std::string getErrorPage(int statusCode) const;
    [[nodiscard]] std::string getCGIPath(const std::string &extension) const;
    [[nodiscard]] bool isCGI(const std::string &extension) const;

    [[nodiscard]] bool has(const std::string &name) const;
    [[nodiscard]] bool owns(const std::string &name) const;

    [[nodiscard]] const ADirective *getDirective(const std::string &name) const;
    [[nodiscard]] std::vector<const ADirective *> getDirectives() const;

    template <typename T> std::optional<T> get(const std::string &name) const
    {
        const auto *directive = getDirective(name);
        if (!directive)
        {
            return std::nullopt;
        }
        return directive->getValue().try_get<T>();
    }

    [[nodiscard]] const std::string &getBaseDir() const; // todo: maybe no

    void setBaseDir(const std::string &dir);

    [[nodiscard]] std::string resolvePath(const std::string &path) const;

  protected:
    virtual void parseBlock(const std::string &block) = 0;
    void parseDirectives(const std::string &declarations);
    std::vector<std::unique_ptr<ADirective>>
        directives_;                  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
    const AConfig *parent_ = nullptr; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
    std::string baseDir_{};           // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};