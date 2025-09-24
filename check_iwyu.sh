#!/bin/bash

# Don't exit on first error - we want to continue checking all files
# set -e

# Detect project root - try container path first, then current directory
if [ -d "/workspace" ]; then
    PROJECT_ROOT="/workspace"
else
    PROJECT_ROOT="$(pwd)"
fi

# Find the build directory - check multiple possible locations
BUILD_DIR=""
echo -e "${BLUE}🔍 Looking for build directory with compile_commands.json...${NC}"
for build_candidate in "$PROJECT_ROOT/build-container" "$PROJECT_ROOT/build-local" "$PROJECT_ROOT/build"; do
    echo -e "  Checking: $build_candidate"
    if [ -d "$build_candidate" ] && [ -f "$build_candidate/compile_commands.json" ]; then
        BUILD_DIR="$build_candidate"
        echo -e "${GREEN}  ✅ Found!${NC}"
        break
    else
        echo -e "${YELLOW}  ❌ Not found or no compile_commands.json${NC}"
    fi
done

IWYU_MAPPING="$PROJECT_ROOT/.iwyu.imp"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🔍 Running Include What You Use Analysis...${NC}"
echo -e "${BLUE}📁 Project root: $PROJECT_ROOT${NC}"

# Check if IWYU is available (try both common names)
IWYU_CMD=""
if command -v include-what-you-use >/dev/null 2>&1; then
    IWYU_CMD="include-what-you-use"
    echo -e "${GREEN}✅ Found IWYU as: include-what-you-use${NC}"
elif command -v iwyu >/dev/null 2>&1; then
    IWYU_CMD="iwyu"
    echo -e "${GREEN}✅ Found IWYU as: iwyu${NC}"
else
    echo -e "${RED}❌ IWYU not found. Please install it first.${NC}"
    echo -e "${YELLOW}💡 Try: sudo apt install iwyu  or  yay -S include-what-you-use${NC}"
    exit 1
fi

echo -e "${BLUE}🛠️  Using IWYU command: $IWYU_CMD${NC}"

# Check if mapping file exists
if [ -f "$IWYU_MAPPING" ]; then
    echo -e "${GREEN}📋 Using IWYU mapping file: $IWYU_MAPPING${NC}"
else
    echo -e "${YELLOW}⚠️  No IWYU mapping file found at: $IWYU_MAPPING${NC}"
    echo -e "${YELLOW}💡 Consider creating one for better IWYU suggestions${NC}"
fi

# Check if we found a build directory
if [ -z "$BUILD_DIR" ]; then
    echo -e "${YELLOW}⚠️  No build directory with compile_commands.json found.${NC}"
    echo -e "${YELLOW}📂 Checked: build-container/, build-local/, build/${NC}"
    echo -e "${YELLOW}🔨 Running cmake to create build directory...${NC}"
    
    cd "$PROJECT_ROOT"
    # Try to create build directory (prefer build-container in container, build-local otherwise)
    if [ -d "/workspace" ]; then
        BUILD_DIR="$PROJECT_ROOT/build-container"
    else
        BUILD_DIR="$PROJECT_ROOT/build-local"
    fi
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
else
    echo -e "${GREEN}✅ Found build directory: $BUILD_DIR${NC}"
fi

# Final check that compile_commands.json exists
if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
    echo -e "${RED}❌ Failed to create compile_commands.json in $BUILD_DIR${NC}"
    cd "$PROJECT_ROOT"
    cmake -B "$BUILD_DIR" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
    
    # Check again after cmake
    if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
        echo -e "${RED}❌ Still no compile_commands.json after cmake. Exiting.${NC}"
        exit 1
    fi
fi

# Create results directory
RESULTS_DIR="$PROJECT_ROOT/iwyu_results"
mkdir -p "$RESULTS_DIR"

# Function to run IWYU on a single file
run_iwyu_on_file() {
    local file="$1"
    local relative_path="${file#$PROJECT_ROOT/}"
    local output_file="$RESULTS_DIR/$(basename "$file" .cpp).iwyu"
    
    echo -e "${BLUE}Analyzing: ${relative_path}${NC}"
    
    # Run IWYU with compile commands and mapping file
    if "$IWYU_CMD" \
        -I"$PROJECT_ROOT" \
        -std=c++20 \
        -Xiwyu --verbose=3 \
        -Xiwyu --quoted_includes_first \
        -Xiwyu --cxx17ns \
        -Xiwyu --mapping_file="$IWYU_MAPPING" \
        "$file" \
        2>&1 | tee "$output_file"; then
        
        # Check if IWYU found issues
        if grep -q "should add these lines:" "$output_file" || grep -q "should remove these lines:" "$output_file"; then
            echo -e "${YELLOW}⚠️  Issues found in $relative_path${NC}"
            return 2  # Issues found (not a script failure)
        else
            echo -e "${GREEN}✅ $relative_path looks good${NC}"
            return 0  # All good
        fi
    else
        local iwyu_exit_code=$?
        echo -e "${RED}❌ IWYU failed for $relative_path (exit code: $iwyu_exit_code)${NC}"
        return 1  # IWYU execution failed
        return 1
    fi
}

# Find all C++ source files
echo -e "\n${BLUE}Finding C++ source files...${NC}"
cpp_files=()
while IFS= read -r -d '' file; do
    cpp_files+=("$file")
done < <(find "$PROJECT_ROOT/webserv" -name "*.cpp" -print0 2>/dev/null)

if [ ${#cpp_files[@]} -eq 0 ]; then
    echo -e "${RED}❌ No .cpp files found in webserv directory${NC}"
    exit 1
fi

echo -e "${BLUE}Found ${#cpp_files[@]} C++ source files${NC}\n"

# Run IWYU on all files
issues_found=0
total_files=${#cpp_files[@]}
current_file=0

for file in "${cpp_files[@]}"; do
    ((current_file++))
    echo -e "${BLUE}[$current_file/$total_files]${NC}"
    
    run_iwyu_on_file "$file"
    exit_code=$?
    
    if [ $exit_code -eq 2 ]; then
        # Issues found (normal)
        ((issues_found++))
    elif [ $exit_code -eq 1 ]; then
        # IWYU execution failed (error)  
        echo -e "${RED}⚠️  IWYU execution error for $(basename "$file")${NC}"
        ((issues_found++))
    fi
    # exit_code 0 means no issues found
    
    echo ""
done

# Summary
echo -e "${BLUE}📊 IWYU Analysis Summary${NC}"
echo -e "Total files analyzed: $total_files"
echo -e "Files with issues: $issues_found"
echo -e "Results saved in: $RESULTS_DIR"

if [ $issues_found -eq 0 ]; then
    echo -e "${GREEN}🎉 All files have proper includes!${NC}"
    exit 0
else
    echo -e "${YELLOW}⚠️  $issues_found files need attention${NC}"
    echo -e "${BLUE}💡 Run './fix_iwyu.sh' to review suggested fixes${NC}"
    exit 0  # Don't fail the script for include suggestions
fi