
cmake_minimum_required(VERSION 3.10)
project(webserv)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Enable parallel compilation
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
endif()

# Add source files
file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/webserv/*.cpp"
)

# Remove main.cpp from sources for library (we'll add it back for the main executable)
list(FILTER SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Add include directories
include_directories(
    ${PROJECT_SOURCE_DIR}
)


# Build type options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Define available build types for IDE
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug"
    "Release"
    "ASAN"
    "RelWithDebInfo"
    "MinSizeRel"
)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Set build flags for different build types
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build: adding debug flags")
    add_compile_options(-g -O0)
    add_definitions(-DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release build: adding optimization flags")
    add_compile_options(-O3)
    add_definitions(-DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "ASAN")
    message(STATUS "AddressSanitizer build: adding sanitizer flags")
    add_compile_options(-g -O1 -fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
    add_definitions(-DASAN)
endif()

# Add executable target
add_executable(webserv ${SOURCES} "${PROJECT_SOURCE_DIR}/webserv/main.cpp")

# Create a library for testing (without main.cpp)
add_library(webserv_lib ${SOURCES})

# Google Test integration
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GTEST gtest)
        pkg_check_modules(GTEST_MAIN gtest_main)
    endif()

    if(GTEST_FOUND AND GTEST_MAIN_FOUND)
        message(STATUS "Using system Google Test")
        # Use system gtest - variables will be set by pkg_check_modules
    else()
        message(STATUS "Downloading Google Test")
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        )
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)

        # Make sure gtest targets are available
        if(TARGET gtest_main)
            message(STATUS "Google Test targets available")
        endif()
    endif()

    # Add test directory if it exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests")
        add_subdirectory(tests)
    else()
        message(STATUS "Tests directory not found, creating basic test structure")
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/tests")

        # Create a basic test file
        file(WRITE "${CMAKE_SOURCE_DIR}/tests/test_main.cpp"
"#include <gtest/gtest.h>

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
")

        # Create CMakeLists.txt for tests
        file(WRITE "${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt"
"# Test executable
add_executable(webserv_tests
    test_main.cpp
)

# Link with Google Test
if(TARGET gtest_main)
    target_link_libraries(webserv_tests
        gtest_main
        webserv_lib
    )
else()
    target_link_libraries(webserv_tests
        \${GTEST_LIBRARIES}
        \${GTEST_MAIN_LIBRARIES}
        webserv_lib
    )
    target_include_directories(webserv_tests PRIVATE \${GTEST_INCLUDE_DIRS})
endif()

target_include_directories(webserv_tests PRIVATE
    \${CMAKE_SOURCE_DIR}
    \${CMAKE_SOURCE_DIR}/webserv
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(webserv_tests)
")
    endif()
endif()

