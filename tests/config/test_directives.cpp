#include <webserv/config/directive/BoolDirective.hpp>
#include <webserv/config/directive/DirectiveFactory.hpp>
#include <webserv/config/directive/IntDirective.hpp>
#include <webserv/config/directive/StringDirective.hpp>

#include <gtest/gtest.h>

/**
 * @file test_directives.cpp
 * @brief Unit tests for directive classes
 */

class DirectiveTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Setup code if needed
    }

    void TearDown() override
    {
        // Cleanup code if needed
    }
};

TEST_F(DirectiveTest, StringDirectiveCreation)
{
    StringDirective directive("server_name", "localhost");

    EXPECT_EQ(directive.getName(), "server_name");
    EXPECT_TRUE(directive.getValue().holds<std::string>());
    EXPECT_EQ(directive.getValue().get<std::string>(), "localhost");
}

TEST_F(DirectiveTest, StringDirectiveParse)
{
    StringDirective directive("root", "");
    directive.parse("/var/www/html");

    EXPECT_EQ(directive.getValueAs<std::string>(), "/var/www/html");
}

TEST_F(DirectiveTest, IntDirectiveCreation)
{
    IntDirective directive("listen", "8080");

    EXPECT_EQ(directive.getName(), "listen");
    EXPECT_TRUE(directive.getValue().holds<int>());
    EXPECT_EQ(directive.getValue().get<int>(), 8080);
}

TEST_F(DirectiveTest, IntDirectiveParse)
{
    IntDirective directive("port", "0");
    directive.parse("9000");

    EXPECT_EQ(directive.getValueAs<int>(), 9000);
}

TEST_F(DirectiveTest, BoolDirectiveCreation)
{
    BoolDirective directive("autoindex", "on");

    EXPECT_EQ(directive.getName(), "autoindex");
    EXPECT_TRUE(directive.getValue().holds<bool>());
    EXPECT_TRUE(directive.getValue().get<bool>());
}

TEST_F(DirectiveTest, BoolDirectiveParsing)
{
    BoolDirective directive("test", "off");

    directive.parse("on");
    EXPECT_TRUE(directive.getValueAs<bool>());

    directive.parse("off");
    EXPECT_FALSE(directive.getValueAs<bool>());

    directive.parse("true");
    EXPECT_TRUE(directive.getValueAs<bool>());

    directive.parse("false");
    EXPECT_FALSE(directive.getValueAs<bool>());
}

TEST_F(DirectiveTest, DirectiveFactoryCreateStringDirective)
{
    auto directive = DirectiveFactory::createDirective("server_name example.com");

    ASSERT_NE(directive, nullptr);
    EXPECT_EQ(directive->getName(), "server_name");
    EXPECT_TRUE(directive->getValue().holds<std::vector<std::string>>());
}

TEST_F(DirectiveTest, DirectiveFactoryCreateIntDirective)
{
    auto directive = DirectiveFactory::createDirective("listen 8080");

    ASSERT_NE(directive, nullptr);
    EXPECT_EQ(directive->getName(), "listen");
    EXPECT_TRUE(directive->getValue().holds<int>());
    EXPECT_EQ(directive->getValue().get<int>(), 8080);
}

TEST_F(DirectiveTest, DirectiveFactoryCreateBoolDirective)
{
    auto directive = DirectiveFactory::createDirective("autoindex on");

    ASSERT_NE(directive, nullptr);
    EXPECT_EQ(directive->getName(), "autoindex");
    EXPECT_TRUE(directive->getValue().holds<bool>());
    EXPECT_TRUE(directive->getValue().get<bool>());
}

TEST_F(DirectiveTest, DirectiveFactoryInvalidDirective)
{
    // DirectiveFactory throws std::invalid_argument for unknown directives
    EXPECT_THROW(DirectiveFactory::createDirective("invalid_directive_name value"), std::invalid_argument);
}