#include <iostream>

#include <gtest/gtest.h>

/**
 * @file test_main.cpp
 * @brief Main entry point for webserv unit tests
 */

int main(int argc, char **argv)
{
    std::cout << "Running webserv unit tests...\n";

    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // Run all tests
    return RUN_ALL_TESTS();
}