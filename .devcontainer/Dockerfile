FROM ubuntu:22.04

# Set environment variables for non-interactive installation
ENV DEBIAN_FRONTEND=noninteractive

# Install essential development tools
RUN apt-get update && apt-get install -y \
    # Build essentials
    build-essential \
    clang \
    clang-format \
    clang-tidy \
    clangd \
    lldb \
    # Build tools
    cmake \
    make \
    ninja-build \
    pkg-config \
    # Version control and utilities
    git \
    wget \
    curl \
    unzip \
    # Development utilities  
    gdb \
    valgrind \
    strace \
    # Network tools for web server testing
    netcat \
    telnet \
    # Text processing
    vim \
    nano \
    # Clean up
    && rm -rf /var/lib/apt/lists/*

# Create a non-root user for development
RUN groupadd --gid 1000 vscode \
    && useradd --uid 1000 --gid vscode --shell /bin/bash --create-home vscode \
    && apt-get update \
    && apt-get install -y sudo \
    && echo vscode ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/vscode \
    && chmod 0440 /etc/sudoers.d/vscode \
    && rm -rf /var/lib/apt/lists/*

# Set clang as the default compiler
ENV CC=clang
ENV CXX=clang++

# Set working directory
WORKDIR /workspace

# Switch to non-root user
USER vscode