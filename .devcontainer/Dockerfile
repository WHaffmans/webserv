FROM ubuntu:22.04

# Set environment variables for non-interactive installation
ENV DEBIAN_FRONTEND=noninteractive

# Install essential development tools
RUN apt-get update && apt-get install -y \
    # Build essentials
    build-essential \
    # Add LLVM repository for specific Clang versions
    wget \
    lsb-release \
    software-properties-common \
    gnupg \
    && rm -rf /var/lib/apt/lists/*

# Install development tools - Ubuntu 22.04 approach
RUN apt-get update && apt-get install -y \
    # Build essentials
    build-essential \
    # Check what Clang versions are available and install Clang 12 if possible
    software-properties-common \
    wget \
    gnupg \
    && apt-get update && \
    # Install available Clang tools (Ubuntu 22.04 has clang-12 available)
    apt-get install -y \
    clang-12 \
    clang++-12 \
    clang-format-12 \
    clang-tidy-12 \
    # clangd and lldb may have different versioning
    clangd \
    lldb \
    # Build tools
    cmake \
    make \
    ninja-build \
    pkg-config \
    # Version control and utilities
    git \
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
    # Include What You Use
    iwyu \
    # Clean up
    && rm -rf /var/lib/apt/lists/*

# Create symbolic links to make clang-12 the default
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-12 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-12 100 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-12 100 \
    && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-12 100

# Create a non-root user for development
RUN groupadd --gid 1000 vscode \
    && useradd --uid 1000 --gid vscode --shell /bin/bash --create-home vscode \
    && apt-get update \
    && apt-get install -y sudo \
    && echo vscode ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/vscode \
    && chmod 0440 /etc/sudoers.d/vscode \
    && rm -rf /var/lib/apt/lists/*

# Set clang-12 as the default compiler
ENV CC=clang-12
ENV CXX=clang++-12

# Set working directory
WORKDIR /workspace

# Switch to non-root user
USER vscode