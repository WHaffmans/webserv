#!/bin/bash

# Webserv Code Coverage Script
# This script builds the project with coverage enabled and generates a coverage report

set -e  # Exit on any error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build_coverage"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}Webserv Code Coverage Generator${NC}"
echo "=================================="

# Check for required tools
echo -e "${YELLOW}Checking for required tools...${NC}"

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: CMake not found. Please install CMake.${NC}"
    exit 1
fi

# Check for compiler
if command -v g++ &> /dev/null; then
    COMPILER="g++"
    echo -e "${GREEN}Found compiler: $COMPILER${NC}"
elif command -v clang++ &> /dev/null; then
    COMPILER="clang++"
    echo -e "${GREEN}Found compiler: $COMPILER${NC}"
else
    echo -e "${RED}Error: No suitable C++ compiler found (g++ or clang++)${NC}"
    exit 1
fi

# Check for coverage tools
COVERAGE_TOOL=""
if command -v lcov &> /dev/null && command -v genhtml &> /dev/null; then
    COVERAGE_TOOL="lcov"
    echo -e "${GREEN}Found coverage tool: lcov${NC}"
elif command -v gcovr &> /dev/null; then
    COVERAGE_TOOL="gcovr"
    echo -e "${GREEN}Found coverage tool: gcovr${NC}"
else
    echo -e "${YELLOW}Warning: No coverage report tools found.${NC}"
    echo -e "${YELLOW}Install with: sudo apt-get install lcov${NC}"
    echo -e "${YELLOW}Or:           pip install gcovr${NC}"
    echo -e "${YELLOW}Coverage data will still be generated.${NC}"
fi

# Clean previous build
echo -e "${YELLOW}Cleaning previous coverage build...${NC}"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Configure with coverage
echo -e "${YELLOW}Configuring build with coverage enabled...${NC}"
cd "$BUILD_DIR"

cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DENABLE_COVERAGE=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
echo -e "${YELLOW}Building project...${NC}"
make -j$(nproc)

# Run tests with coverage
echo -e "${YELLOW}Running tests with coverage...${NC}"
if make coverage; then
    echo -e "${GREEN}Coverage report generated successfully!${NC}"
    
    # Show where to find the report
    if [ "$COVERAGE_TOOL" = "lcov" ]; then
        REPORT_PATH="$BUILD_DIR/coverage_report/index.html"
        if [ -f "$REPORT_PATH" ]; then
            echo -e "${GREEN}Coverage report available at: $REPORT_PATH${NC}"
            echo -e "${BLUE}Open with: firefox $REPORT_PATH${NC}"
        fi
    elif [ "$COVERAGE_TOOL" = "gcovr" ]; then
        REPORT_PATH="$BUILD_DIR/coverage_report.html"
        if [ -f "$REPORT_PATH" ]; then
            echo -e "${GREEN}Coverage report available at: $REPORT_PATH${NC}"
            echo -e "${BLUE}Open with: firefox $REPORT_PATH${NC}"
        fi
    fi
    
    # Show summary
    echo -e "${YELLOW}Coverage Summary:${NC}"
    make coverage_summary 2>/dev/null || echo "Summary not available"
    
else
    echo -e "${RED}Coverage generation failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Coverage analysis complete!${NC}"
