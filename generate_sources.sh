#!/bin/bash
# Generate sources.mk file with all .cpp files

SOURCES_FILE="sources.mk"

echo "# Auto-generated source files" > "$SOURCES_FILE"
echo "SRCS = \\" >> "$SOURCES_FILE"

find webserv -name "*.cpp" -type f | sort | while read -r file; do
    echo "       $file \\" >> "$SOURCES_FILE"
done

# Remove trailing backslash from last line
sed -i '$ s/ \\$//' "$SOURCES_FILE"

echo "Generated $SOURCES_FILE with $(grep -c "\.cpp" "$SOURCES_FILE") source files"
