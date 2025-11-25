#include <gtest/gtest.h>

// Basic test to verify Google Test is working
TEST(BasicTest, TruthTest) {
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
    EXPECT_EQ(1 + 1, 2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
