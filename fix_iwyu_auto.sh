#!/bin/bash

# Automatic IWYU Fix using iwyu-fix-includes
# Uses the official iwyu-fix-includes tool to automatically apply IWYU suggestions

set -e

# Find project root (directory containing this script)
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RESULTS_DIR="$PROJECT_ROOT/iwyu_results"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🔧 Automatic IWYU Fix using iwyu-fix-includes${NC}"
echo -e "${YELLOW}⚠️  This will automatically apply all IWYU suggestions${NC}"

# Check if iwyu-fix-includes is available
if ! command -v iwyu-fix-includes &> /dev/null; then
    echo -e "${RED}❌ iwyu-fix-includes not found. Please install it:${NC}"
    echo -e "${YELLOW}   # On Ubuntu/Debian:${NC}"
    echo -e "${YELLOW}   sudo apt install iwyu${NC}"
    echo -e "${YELLOW}   # On Arch Linux:${NC}"
    echo -e "${YELLOW}   sudo pacman -S include-what-you-use${NC}"
    echo -e "${YELLOW}   # Or build from source${NC}"
    exit 1
fi

if [ ! -d "$RESULTS_DIR" ]; then
    echo -e "${RED}❌ No IWYU results found. Run './check_iwyu.sh' first.${NC}"
    exit 1
fi

# Make sure we can build first
echo -e "${BLUE}🔍 Testing initial build...${NC}"
if ! make -j$(nproc) release >/dev/null 2>&1; then
    echo -e "${RED}❌ Project doesn't build currently. Fix build issues first.${NC}"
    exit 1
fi
echo -e "${GREEN}✅ Initial build successful${NC}"

# Count IWYU result files
result_count=$(find "$RESULTS_DIR" -name "*.iwyu" -type f | wc -l)
if [ "$result_count" -eq 0 ]; then
    echo -e "${YELLOW}⚠️  No .iwyu files found in $RESULTS_DIR${NC}"
    exit 1
fi

echo -e "${BLUE}📁 Found $result_count IWYU result files${NC}"

# Apply all fixes using iwyu-fix-includes
echo -e "${BLUE}🔧 Applying IWYU fixes...${NC}"

# iwyu-fix-includes options:
# --comments: Add comments explaining why headers are included
# --update_comments: Update existing IWYU comments
# --safe_headers: Only remove headers that are definitely safe to remove
iwyu_fix_options=""

# Ask user for options
# echo -e "${YELLOW}Choose fix options:${NC}"
# echo -e "  1) ${GREEN}Safe mode${NC} (conservative, only safe changes)"
# echo -e "  2) ${BLUE}Standard mode${NC} (recommended)"
# echo -e "  3) ${YELLOW}Aggressive mode${NC} (with comments, updates existing)"
# echo -n "Choose (1-3) [default: 2]: "

# if [ -t 0 ]; then  # Only read input if running interactively
#     read -r choice
# else
    choice="3"
# fi

case "${choice:-2}" in
    1)
        iwyu_fix_options="--safe_headers"
        echo -e "${GREEN}Using safe mode${NC}"
        ;;
    3)
        iwyu_fix_options="--comments --update_comments"
        echo -e "${YELLOW}Using aggressive mode with comments${NC}"
        ;;
    *)
        iwyu_fix_options=""
        echo -e "${BLUE}Using standard mode${NC}"
        ;;
esac

# Apply the fixes
echo -e "${BLUE}🚀 Running iwyu-fix-includes...${NC}"

# iwyu-fix-includes expects all IWYU output concatenated on stdin
temp_output=$(mktemp)
cat "$RESULTS_DIR"/*.iwyu > "$temp_output"

if cat "$temp_output" | iwyu-fix-includes $iwyu_fix_options; then
    echo -e "${GREEN}✅ IWYU fixes applied successfully!${NC}"
    
    # Clean up
    rm -f "$temp_output"
    
    # Test build after fixes
    echo -e "${BLUE}🔍 Testing build after fixes...${NC}"
    if make -j$(nproc) release >/dev/null 2>&1; then
        echo -e "${GREEN}✅ Build successful after applying fixes!${NC}"
        
        # Show what changed
        if command -v git &> /dev/null && git rev-parse --git-dir > /dev/null 2>&1; then
            echo -e "${BLUE}📋 Files modified:${NC}"
            git diff --name-only | head -10
            if [ "$(git diff --name-only | wc -l)" -gt 10 ]; then
                echo -e "${YELLOW}   ... and $(( $(git diff --name-only | wc -l) - 10 )) more files${NC}"
            fi
            
            echo -e "${BLUE}💡 Next steps:${NC}"
            echo -e "  • Review changes: ${BLUE}git diff${NC}"
            echo -e "  • Run tests: ${BLUE}make test${NC} (if available)"
            echo -e "  • Commit: ${BLUE}git add -A && git commit -m 'fix: apply IWYU suggestions'${NC}"
        fi
    else
        echo -e "${RED}❌ Build failed after applying fixes!${NC}"
        echo -e "${YELLOW}💡 You may need to manually fix some issues or revert changes${NC}"
        rm -f "$temp_output"
        exit 1
    fi
    
else
    echo -e "${RED}❌ iwyu-fix-includes failed!${NC}"
    echo -e "${YELLOW}💡 Check the error output above for details${NC}"
    rm -f "$temp_output"
    exit 1
fi

echo -e "${GREEN}🎉 IWYU fix process completed!${NC}"