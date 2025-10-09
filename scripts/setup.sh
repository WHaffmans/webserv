#!/bin/bash

# Development Environment Setup Script
# This script sets up the build environment for both local and container development

set -e  # Exit on any error

# Change to project root directory
cd "$(dirname "$0")/.." || exit 1

echo "🚀 Setting up webserv development environment..."

# Detect environment
if [ "$(whoami)" = "vscode" ]; then
    BUILD_DIR="build-container"
    ENVIRONMENT="Dev Container"
else
    BUILD_DIR="build-local"
    ENVIRONMENT="Local"
fi

echo "📍 Environment detected: $ENVIRONMENT"
echo "🔧 Using build directory: $BUILD_DIR"

# Clean old build directories if they exist
echo "🧹 Cleaning old build artifacts..."
rm -rf build build-* 2>/dev/null || true

# Create fresh build directory
echo "📁 Creating build directory: $BUILD_DIR"
cmake -B "$BUILD_DIR" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_BUILD_TYPE=Release

# Create symlink for compile_commands.json at workspace root for clangd
if [ -f "$BUILD_DIR/compile_commands.json" ]; then
    echo "🔗 Creating compile_commands.json symlink for clangd..."
    ln -sf "$BUILD_DIR/compile_commands.json" compile_commands.json
fi

echo "✅ Environment setup complete!"
echo ""
echo "Next steps:"
echo "  • Run 'make all' to build the project"
echo "  • Run 'make run' to start the server"
echo "  • Use VS Code tasks (Ctrl+Shift+P > Tasks: Run Task) for build operations"