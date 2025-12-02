#!/bin/bash

# Distribution Package Creator
# Creates a clean distribution package in the dist/ folder

set -e  # Exit on any error

# Change to project root directory
cd "$(dirname "$0")/.." || exit 1

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

DIST_DIR="dist"

echo -e "${BLUE}📦 Creating distribution package...${NC}"

# Clean old dist directory
if [ -d "$DIST_DIR" ]; then
    echo -e "${YELLOW}🧹 Cleaning old distribution...${NC}"
    rm -rf "$DIST_DIR"
fi

# Create dist directory
echo -e "${BLUE}📁 Creating dist directory...${NC}"
mkdir -p "$DIST_DIR"

# Copy source files
echo -e "${BLUE}📄 Copying source files...${NC}"
mkdir -p "$DIST_DIR/src"
cp -r src/webserv "$DIST_DIR/src/"

# Copy docker folder
echo -e "${BLUE}🐳 Copying docker configuration...${NC}"
cp -r docker "$DIST_DIR/"
rm -rf "$DIST_DIR/docker/.git"

# Copy config folder
echo -e "${BLUE}⚙️  Copying config files...${NC}"
cp -r config "$DIST_DIR/"

# Copy htdocs folder
echo -e "${BLUE}🌐 Copying htdocs...${NC}"
cp -r htdocs "$DIST_DIR/"

# Copy webserv-tester folder
echo -e "${BLUE}🧪 Copying webserv-tester...${NC}"
cp -r webserv-tester "$DIST_DIR/"
rm -rf "$DIST_DIR/webserv-tester/.git"

# Update SERVER_PATH in the copied run_test.sh
echo -e "${BLUE}🔧 Updating SERVER_PATH in webserv-tester/run_test.sh...${NC}"
sed -i 's|SERVER_PATH="../build/webserv"|SERVER_PATH="../webserv"|g' "$DIST_DIR/webserv-tester/run_test.sh"

# Update webserv_path in the copied invalid_config_tests.py
echo -e "${BLUE}🔧 Updating webserv_path in webserv-tester/tests_suites/invalid_config_tests.py...${NC}"
sed -i 's|self.webserv_path = Path("../build/webserv")|self.webserv_path = Path("../webserv")|g' "$DIST_DIR/webserv-tester/tests_suites/invalid_config_tests.py"

# Update Dockerfile to use correct webserv binary path (Makefile builds to ./ not build/)
echo -e "${BLUE}🔧 Updating Dockerfile webserv binary path...${NC}"
sed -i 's|COPY --from=0 /tmp/build/webserv /webserv/.|COPY --from=0 /tmp/webserv /webserv/.|g' "$DIST_DIR/docker/webserv/Dockerfile"

# Copy sources.mk
echo -e "${BLUE}📋 Copying sources.mk...${NC}"
cp sources.mk "$DIST_DIR/"

# Copy Makefile.old as Makefile
echo -e "${BLUE}🔨 Copying Makefile.old as Makefile...${NC}"
cp Makefile.old "$DIST_DIR/Makefile"

echo -e "${GREEN}✅ Distribution package created successfully in ${DIST_DIR}/${NC}"
echo -e "${BLUE}📦 Contents:${NC}"
ls -lh "$DIST_DIR"

