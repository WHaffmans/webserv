#include <webserv/http/HttpResponse.hpp>
#include <gtest/gtest.h>

TEST(HttpResponseTest, BasicTest) {
    HttpResponse response;
    EXPECT_FALSE(response.isComplete());
}
