#!/bin/bash

# Script to test webserv with invalid configuration files
# Expects webserv to exit with code 1 for invalid configs

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
WEBSERV_BINARY="./build/webserv"
CONFIG_DIR="./config/invalid_configs"
TIMEOUT=5  # Timeout in seconds for webserv to start/fail

# Check if webserv binary exists
if [[ ! -f "$WEBSERV_BINARY" ]]; then
    echo -e "${RED}Error: webserv binary not found at $WEBSERV_BINARY${NC}"
    echo "Please build webserv first with: make release"
    exit 1
fi

# Check if config directory exists
if [[ ! -d "$CONFIG_DIR" ]]; then
    echo -e "${RED}Error: Invalid config directory not found at $CONFIG_DIR${NC}"
    exit 1
fi

# Function to test a single config file
test_config() {
    local config_file="$1"
    local config_name=$(basename "$config_file")
    
    echo -n "Testing $config_name... "
    
    # Run webserv with timeout to prevent hanging
    # Use timeout command to kill if it runs too long
    if timeout "$TIMEOUT" "$WEBSERV_BINARY" "$config_file" >/dev/null 2>&1; then
        # If timeout succeeds, webserv exited normally (bad - should have failed)
        local exit_code=$?
        if [[ $exit_code -eq 0 ]]; then
            echo -e "${RED}FAIL (unexpectedly succeeded)${NC}"
            return 1
        else
            echo -e "${GREEN}PASS (exit code: $exit_code)${NC}"
            return 0
        fi
    else
        # Check the exit code from timeout
        local timeout_exit_code=$?
        if [[ $timeout_exit_code -eq 124 ]]; then
            # Timeout occurred - webserv is hanging (bad for invalid config)
            echo -e "${RED}FAIL (hanging - probably valid config)${NC}"
            return 1
        elif [[ $timeout_exit_code -eq 1 ]]; then
            # Webserv exited with error code 1 (good)
            echo -e "${GREEN}PASS (validation failed as expected)${NC}"
            return 0
        else
            # Other exit code
            echo -e "${YELLOW}UNCERTAIN (exit code: $timeout_exit_code)${NC}"
            return 1
        fi
    fi
}

# Main testing loop
echo "Testing webserv with invalid configuration files..."
echo "Expected behavior: webserv should exit with code 1 for all invalid configs"
echo "=================================================="

total_tests=0
passed_tests=0
failed_tests=0

# Test each .conf file in the invalid_configs directory
for config_file in "$CONFIG_DIR"/*.conf; do
    if [[ -f "$config_file" ]]; then
        total_tests=$((total_tests + 1))
        
        if test_config "$config_file"; then
            passed_tests=$((passed_tests + 1))
        else
            failed_tests=$((failed_tests + 1))
        fi
    fi
done

# Test results summary
echo "=================================================="
echo "Test Summary:"
echo -e "Total tests: $total_tests"
echo -e "${GREEN}Passed: $passed_tests${NC}"
echo -e "${RED}Failed: $failed_tests${NC}"

if [[ $failed_tests -eq 0 ]]; then
    echo -e "\n${GREEN}✓ All tests passed! Config validation is working correctly.${NC}"
    exit 0
else
    echo -e "\n${RED}✗ Some tests failed. Check config validation implementation.${NC}"
    exit 1
fi