
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
option(ENABLE_COVERAGE "Enable code coverage" OFF)

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

# Coverage configuration
if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")
    
    # Find required tools
    find_program(GCOV_PATH gcov)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    find_program(GCOVR_PATH gcovr)
    
    if(NOT GCOV_PATH)
        message(FATAL_ERROR "gcov not found! Please install gcov. On Ubuntu: sudo apt-get install gcc")
    endif()
    
    # Check for coverage tools (prefer lcov, fallback to gcovr)
    if(LCOV_PATH AND GENHTML_PATH)
        message(STATUS "Found lcov and genhtml - using lcov for coverage reports")
        set(COVERAGE_TOOL "lcov")
    elseif(GCOVR_PATH)
        message(STATUS "Found gcovr - using gcovr for coverage reports")
        set(COVERAGE_TOOL "gcovr")
    else()
        message(STATUS "Coverage tools not found. Install with:")
        message(STATUS "  Ubuntu: sudo apt-get install lcov")
        message(STATUS "  Or:     pip install gcovr")
        message(STATUS "Coverage data will still be generated, but no HTML report")
        set(COVERAGE_TOOL "none")
    endif()
    
    # Set coverage flags based on compiler
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(STATUS "Using GCC coverage flags")
        set(COVERAGE_FLAGS "--coverage -fprofile-arcs -ftest-coverage -fno-inline -fno-inline-small-functions -fno-default-inline")
        set(COVERAGE_LINK_FLAGS "--coverage")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Using Clang coverage flags")
        # Use --coverage for Clang to generate gcov-compatible data
        set(COVERAGE_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
        set(COVERAGE_LINK_FLAGS "--coverage")
    else()
        message(FATAL_ERROR "Code coverage only supported with GCC or Clang")
    endif()
    
    # Apply coverage flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COVERAGE_LINK_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${COVERAGE_LINK_FLAGS}")
    
    # Ensure debug information is available
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
endif()

# Set build flags for different build types
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build: adding debug flags")
    add_compile_options(-g -O0)
    add_definitions(-DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release build: adding optimization flags")
    # -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat=2
    add_compile_options(-O2 -g -Wall -Wextra -Werror)
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

# Coverage targets
if(ENABLE_COVERAGE AND BUILD_TESTS)
    # Coverage cleanup target
    add_custom_target(coverage_clean
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/coverage_report
        COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/coverage.info
        COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/coverage.xml
        COMMENT "Cleaning coverage data"
    )
    
    if(COVERAGE_TOOL STREQUAL "lcov")
        # Main coverage target using lcov
        add_custom_target(coverage
            DEPENDS webserv_tests
            
            # Cleanup previous coverage data
            COMMAND ${LCOV_PATH} --directory . --zerocounters
            
            # Run tests
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose
            
            # Capture coverage data
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
            
            # Remove system and test files from coverage
            COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '/opt/*' '*/tests/*' '*/test/*' '*/_deps/*' '*/build/*' --output-file coverage.info
            
            # Generate HTML report
            COMMAND ${GENHTML_PATH} coverage.info --output-directory coverage_report --title "Webserv Coverage Report" --show-details --legend
            
            # Print summary
            COMMAND ${LCOV_PATH} --summary coverage.info
            
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report with lcov"
        )
        
        # Coverage summary target
        add_custom_target(coverage_summary
            DEPENDS webserv_tests
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '/opt/*' '*/tests/*' '*/test/*' '*/_deps/*' '*/build/*' --output-file coverage.info
            COMMAND ${LCOV_PATH} --summary coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage summary"
        )
        
    elseif(COVERAGE_TOOL STREQUAL "gcovr")
        # Coverage target using gcovr
        add_custom_target(coverage
            DEPENDS webserv_tests
            
            # Run tests
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose
            
            # Generate HTML report
            COMMAND ${GCOVR_PATH} --root ${CMAKE_SOURCE_DIR} --exclude-unreachable-branches --exclude '.*test.*' --exclude '.*_deps.*' --html --html-details -o coverage_report.html
            
            # Generate XML report (for CI)
            COMMAND ${GCOVR_PATH} --root ${CMAKE_SOURCE_DIR} --exclude-unreachable-branches --exclude '.*test.*' --exclude '.*_deps.*' --xml -o coverage.xml
            
            # Print summary to console
            COMMAND ${GCOVR_PATH} --root ${CMAKE_SOURCE_DIR} --exclude-unreachable-branches --exclude '.*test.*' --exclude '.*_deps.*'
            
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report with gcovr"
        )
        
        # Coverage summary target
        add_custom_target(coverage_summary
            DEPENDS webserv_tests
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose
            COMMAND ${GCOVR_PATH} --root ${CMAKE_SOURCE_DIR} --exclude-unreachable-branches --exclude '.*test.*' --exclude '.*_deps.*'
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage summary"
        )
        
    else()
        # Basic coverage target without report generation
        add_custom_target(coverage
            DEPENDS webserv_tests
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage data generated. Install lcov or gcovr to generate HTML reports."
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running tests with coverage enabled"
        )
    endif()
    
    # Add coverage dependencies
    if(TARGET coverage)
        add_dependencies(coverage webserv_tests)
    endif()
    
    message(STATUS "Coverage targets available:")
    message(STATUS "  make coverage        - Run tests and generate coverage report")
    message(STATUS "  make coverage_summary - Run tests and show coverage summary")
    message(STATUS "  make coverage_clean  - Clean coverage data")
endif()

