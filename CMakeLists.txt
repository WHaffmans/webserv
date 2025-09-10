
cmake_minimum_required(VERSION 3.10)
project(webserv)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/webserv/*.cpp"
)

# Add include directories
include_directories(
    ${PROJECT_SOURCE_DIR}
)


# Build type options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

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
add_executable(webserv ${SOURCES})

