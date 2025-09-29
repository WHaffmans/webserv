
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
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Add test directory
add_subdirectory(tests)

