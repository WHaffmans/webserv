#!/bin/bash

# Test runner script for webserv
# Usage: ./run_tests.sh [options]
# Options:
#   -v, --verbose    Run tests with verbose output
#   -b, --build-only Build tests but don't run them  
#   -c, --coverage   Build with coverage information (if supported)
#   -h, --help       Show this help message

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default options
VERBOSE=false
BUILD_ONLY=false
COVERAGE=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -b|--build-only)
            BUILD_ONLY=true
            shift
            ;;
        -c|--coverage)
            COVERAGE=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  -v, --verbose    Run tests with verbose output"
            echo "  -b, --build-only Build tests but don't run them"
            echo "  -c, --coverage   Build with coverage information (if supported)"
            echo "  -h, --help       Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}🧪 Webserv Test Runner${NC}"
echo "========================================"

# Check if we're in the right directory
if [[ ! -f "CMakeLists.txt" || ! -d "tests" ]]; then
    echo -e "${RED}❌ Error: Please run this script from the webserv project root${NC}"
    exit 1
fi

# Build the project and tests
echo -e "${YELLOW}🔨 Building project and tests...${NC}"
if $COVERAGE; then
    echo -e "${BLUE}📊 Building with coverage information${NC}"
    make release CMAKE_FLAGS="-DCMAKE_CXX_FLAGS=--coverage -DCMAKE_EXE_LINKER_FLAGS=--coverage"
else
    make release
fi

echo -e "${YELLOW}🔨 Building test executable...${NC}"
make test_build

if $BUILD_ONLY; then
    echo -e "${GREEN}✅ Tests built successfully!${NC}"
    echo -e "${BLUE}💡 To run tests: make test${NC}"
    exit 0
fi

# Run the tests
echo -e "${YELLOW}🏃 Running tests...${NC}"
echo "========================================"

if $VERBOSE; then
    make test_verbose
else
    make test
fi

# Check the exit code
if [[ $? -eq 0 ]]; then
    echo "========================================"
    echo -e "${GREEN}✅ All tests passed!${NC}"
    
    if $COVERAGE; then
        echo -e "${BLUE}📊 Generating coverage report...${NC}"
        # Add coverage report generation here if needed
        echo -e "${BLUE}💡 Coverage files generated in build directory${NC}"
    fi
else
    echo "========================================"
    echo -e "${RED}❌ Some tests failed!${NC}"
    echo -e "${YELLOW}💡 Run with -v/--verbose for detailed output${NC}"
    exit 1
fi