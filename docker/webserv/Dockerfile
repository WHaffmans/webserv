FROM ubuntu:22.04

# Set environment variables for non-interactive installation
ENV DEBIAN_FRONTEND=noninteractive

# Install development tools in one layer
RUN apt-get update && apt-get install -y \
    # Build essentials
    build-essential \
    # Clang 12 toolchain
    clang-12 \
    clang++-12 \
    clang-format-12 \
    clang-tidy-12 \
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
    php-cgi \
    # Clean up
    && rm -rf /var/lib/apt/lists/*

# Set up Clang 12 alternatives
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-12 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-12 100 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-12 100 \
    && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-12 100

# Set environment variables
ENV CC=clang-12
ENV CXX=clang++-12

RUN git clone https://github.com/WHaffmans/webserv.git

RUN cd webserv && make

FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \ 
	php \
	php-cgi \
	&& rm -rf /var/lib/apt/lists/*

RUN mkdir /webserv

COPY --from=0 /webserv/build/webserv /webserv/.

COPY ./conf/default.conf /webserv/.

COPY ./conf/hosts /etc/hosts

# RUN chmod 777 /webserv/default.conf

RUN mkdir /logs && touch /logs/webserv.log

CMD ["./webserv/webserv", "/webserv/default.conf"]
