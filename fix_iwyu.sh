#!/bin/bash

set -e

PROJECT_ROOT="/workspace"
RESULTS_DIR="$PROJECT_ROOT/iwyu_results"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🔧 Reviewing IWYU fixes...${NC}"

if [ ! -d "$RESULTS_DIR" ]; then
    echo -e "${RED}❌ No IWYU results found. Run './check_iwyu.sh' first.${NC}"
    exit 1
fi

# Check if there are any result files
result_files=("$RESULTS_DIR"/*.iwyu)
if [ ! -f "${result_files[0]}" ]; then
    echo -e "${YELLOW}⚠️  No IWYU result files found.${NC}"
    exit 1
fi

echo -e "${BLUE}💡 IWYU Analysis Results - Manual Review Required${NC}"
echo -e "${YELLOW}Note: Automatic fixing requires careful review before applying changes.${NC}\n"

files_with_issues=0

for result_file in "$RESULTS_DIR"/*.iwyu; do
    if [ -f "$result_file" ]; then
        filename=$(basename "$result_file" .iwyu)
        
        # Check if this file has suggestions
        if grep -q "should add these lines:\|should remove these lines:" "$result_file"; then
            ((files_with_issues++))
            
            echo -e "${BLUE}=== $filename.cpp ===${NC}"
            
            # Show additions
            if grep -q "should add these lines:" "$result_file"; then
                echo -e "${GREEN}📥 Suggested additions:${NC}"
                sed -n '/should add these lines:/,/^$/p' "$result_file" | grep -v "should add these lines:" | head -20
                echo ""
            fi
            
            # Show removals
            if grep -q "should remove these lines:" "$result_file"; then
                echo -e "${RED}📤 Suggested removals:${NC}"
                sed -n '/should remove these lines:/,/^$/p' "$result_file" | grep -v "should remove these lines:" | head -20
                echo ""
            fi
            
            # Show full analysis (first 30 lines for context)
            echo -e "${BLUE}📋 Full analysis:${NC}"
            head -30 "$result_file"
            echo -e "${YELLOW}... (see $result_file for complete output)${NC}"
            echo -e "${BLUE}${'='*60}${NC}\n"
        fi
    fi
done

if [ $files_with_issues -eq 0 ]; then
    echo -e "${GREEN}🎉 No issues found in any analyzed files!${NC}"
else
    echo -e "${YELLOW}📊 Summary: $files_with_issues files have suggested changes${NC}"
    echo -e "${BLUE}💡 Tips for applying fixes:${NC}"
    echo -e "  • Review each suggestion carefully"
    echo -e "  • Test compilation after each change"
    echo -e "  • Some suggestions might be false positives"
    echo -e "  • Consider project-specific header policies"
    echo ""
    echo -e "${BLUE}🗂️  Detailed results available in: $RESULTS_DIR${NC}"
fi