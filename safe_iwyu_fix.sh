#!/bin/bash

# Safe automatic IWYU fix with build system validation
# This version applies fixes and validates using your actual build system

# Detect project root
if [ -d "/workspace" ]; then
    PROJECT_ROOT="/workspace"
else
    PROJECT_ROOT="$(pwd)"
fi

RESULTS_DIR="$PROJECT_ROOT/iwyu_results"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🔧 Safe IWYU Auto-Fix with Build Validation${NC}"
echo -e "${YELLOW}⚠️  This will apply fixes one file at a time and validate with your build system${NC}"

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

files_fixed=0
files_processed=0

# Process each .iwyu result file
for result_file in "$RESULTS_DIR"/*.iwyu; do
    [ ! -f "$result_file" ] && continue
    
    # Get the corresponding source file
    base_name=$(basename "$result_file" .iwyu)
    source_file=""
    
    # Find the actual source file
    while IFS= read -r -d '' file; do
        if [[ "$(basename "$file" .cpp)" == "$base_name" ]]; then
            source_file="$file"
            break
        fi
    done < <(find "$PROJECT_ROOT/webserv" -name "*.cpp" -print0)
    
    if [ -z "$source_file" ]; then
        continue
    fi
    
    ((files_processed++))
    relative_path="${source_file#$PROJECT_ROOT/}"
    
    echo -e "\n${BLUE}[$files_processed] Processing: $relative_path${NC}"
    
    # Check if there are actual suggestions
    if ! grep -q "should add these lines:" "$result_file"; then
        echo -e "${GREEN}  ✅ No additions needed${NC}"
        continue
    fi
    
    # Create backup
    backup_file="${source_file}.backup"
    cp "$source_file" "$backup_file"
    
    # Extract and apply only the additions (safer than removals)
    additions_made=false
    
    # Get the lines to add and store in temp file to avoid subshell issues
    temp_includes=$(mktemp)
    awk '/should add these lines:/{flag=1; next} /should remove these lines:|^$/{flag=0} flag && /^#include/{print}' "$result_file" > "$temp_includes"
    
    # Process each include line
    while IFS= read -r include_line; do
        [ -z "$include_line" ] && continue
        
        # Clean up the line (remove any trailing whitespace/comments after //)
        clean_include=$(echo "$include_line" | sed 's|//.*$||' | sed 's/[[:space:]]*$//')
        
        # Check if this exact include is already present (be more strict)
        if grep -F "$clean_include" "$source_file" >/dev/null; then
            echo -e "${YELLOW}  ~ Already present: $clean_include${NC}"
            continue
        fi
        
        # Add the include after the first existing #include
        if sed -i "1,/^#include/ { /^#include/ a\\
$include_line
}" "$source_file"; then
            echo -e "${GREEN}  + Added: $include_line${NC}"
            additions_made=true
        fi
    done < "$temp_includes"
    
    rm -f "$temp_includes"
    
    if [ "$additions_made" = true ]; then
        # Test build with changes
        echo -e "${BLUE}  🔨 Testing build...${NC}"
        if make -j$(nproc) release >/dev/null 2>&1; then
            echo -e "${GREEN}  ✅ Build successful with changes${NC}"
            rm "$backup_file"
            ((files_fixed++))
        else
            echo -e "${RED}  ❌ Build failed, reverting changes${NC}"
            mv "$backup_file" "$source_file"
        fi
    else
        echo -e "${GREEN}  ✅ No new includes to add${NC}"
        rm "$backup_file"
    fi
done

echo -e "\n${BLUE}📊 Safe Auto-fix Summary:${NC}"
echo -e "Files processed: $files_processed"
echo -e "Files successfully modified: $files_fixed"

if [ $files_fixed -gt 0 ]; then
    echo -e "${GREEN}🎉 Applied $files_fixed successful fixes!${NC}"
    echo -e "${BLUE}💡 Next steps:${NC}"
    echo -e "  • Review changes: ${BLUE}git diff${NC}"
    echo -e "  • Run full test: ${BLUE}make clean && make all${NC}"
    echo -e "  • Commit: ${BLUE}git add -A && git commit -m 'fix: add missing includes (IWYU)'${NC}"
else
    echo -e "${GREEN}🎉 No fixes needed - all includes are already optimal!${NC}"
fi

exit 0