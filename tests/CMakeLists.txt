# Test directory structure
cmake_minimum_required(VERSION 3.10)

# Test executable
file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Create test executable
add_executable(webserv_tests ${TEST_SOURCES})

# Link against our library and gtest
if(GTEST_FOUND AND GTEST_MAIN_FOUND)
    # Use system gtest
    target_link_libraries(webserv_tests
        webserv_lib
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
    )
    target_include_directories(webserv_tests PRIVATE
        ${GTEST_INCLUDE_DIRS}
    )
else()
    # Use downloaded gtest
    target_link_libraries(webserv_tests
        webserv_lib
        gtest_main
        gtest
    )
endif()

# Include directories for tests
target_include_directories(webserv_tests PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(webserv_tests)

# Add custom test target for running tests with verbose output
add_custom_target(test_verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS webserv_tests
)