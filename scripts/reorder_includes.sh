#!/bin/bash

# Script to reorder includes in .cpp files:
# 1. Move the corresponding .hpp include to the top of includes
# 2. Add a blank line after it (avoiding multiple blank lines)

# Change to project root directory
cd "$(dirname "$0")/.." || exit 1

find src/webserv -name "*.cpp" | while read -r cpp_file; do
    # Get the base name without extension (e.g., "Client" from "Client.cpp")
    base_name=$(basename "$cpp_file" .cpp)
    
    # Use Python for reliable processing
    python3 << EOF
import re
import sys

cpp_file = "$cpp_file"
base_name = "$base_name"

# Read the file
try:
    with open(cpp_file, 'r') as f:
        lines = f.readlines()
except Exception as e:
    print(f"Error reading {cpp_file}: {e}")
    sys.exit(1)

# Find the corresponding header include pattern
header_pattern = re.compile(rf'#include <.*/{re.escape(base_name)}\.hpp>')
header_include_line = None

# Look for the header include
for line in lines:
    if header_pattern.search(line.strip()):
        header_include_line = line
        break

if not header_include_line:
    print(f"No corresponding header found for: {cpp_file}")
    sys.exit(0)

print(f"Processing: {cpp_file}")

# Process the file
result_lines = []
includes_started = False
header_added = False
last_line_was_empty = False

for i, line in enumerate(lines):
    stripped = line.strip()
    is_empty_line = stripped == ""
    
    # Check if this is the start of includes section
    if stripped.startswith('#include') and not includes_started:
        includes_started = True
        # Add the header include first (with blank line after)
        if not header_added:
            result_lines.append(header_include_line)
            result_lines.append('\n')
            header_added = True
            last_line_was_empty = True
    
    # Skip the header include line (we already added it)
    if stripped.startswith('#include') and header_pattern.search(stripped):
        continue
    
    # Handle empty lines: avoid multiple consecutive empty lines
    if is_empty_line:
        if not last_line_was_empty:
            result_lines.append(line)
            last_line_was_empty = True
    else:
        result_lines.append(line)
        last_line_was_empty = False

# Write the result back
try:
    with open(cpp_file, 'w') as f:
        f.writelines(result_lines)
except Exception as e:
    print(f"Error writing {cpp_file}: {e}")
    sys.exit(1)

EOF

done

echo "Include reordering complete!"