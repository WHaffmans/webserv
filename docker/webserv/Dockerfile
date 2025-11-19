FROM ubuntu:22.04

# Set environment variables for non-interactive installation
ENV DEBIAN_FRONTEND=noninteractive
ARG GIT_COMMIT

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

# RUN echo "Using webserv commit: $GIT_COMMIT"

# # Clone repo and checkout specific commit
# RUN git clone https://github.com/WHaffmans/webserv.git && \
#     cd webserv && \
#     git checkout $GIT_COMMIT

# RUN sed -i 's/static constexpr Log::Level COMPILE_TIME_LOG_LEVEL = Log::Level::Info;/static constexpr Log::Level COMPILE_TIME_LOG_LEVEL = Log::Level::Debug;/' webserv/webserv/log/Log.hpp

RUN mkdir -p /tmp/

COPY . /tmp/

RUN rm -rf /tmp/build/


RUN cd /tmp && make


## RUN STAGE

FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \ 
    php \
    php-cgi \
    curl \
    php-phar \
    php-mysqli \
    php-mbstring \
    php-json \
    php-curl \
    php-xml \
    php-zip \
    php-gd \
    # php-session \
    php-tokenizer \
    mariadb-client \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir /webserv

COPY --from=0 /tmp/build/webserv /webserv/.

COPY ./docker/webserv/conf/default.conf /webserv/.

# Install WP-CLI
RUN curl -O https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar && \
    chmod +x wp-cli.phar && \
    mv wp-cli.phar /usr/local/bin/wp

COPY ./docker/webserv/install-wp.sh /usr/local/bin/.

RUN chmod +x /usr/local/bin/install-wp.sh

WORKDIR /webserv
RUN mkdir logs && touch logs/webserv.log

ENTRYPOINT ["/usr/local/bin/install-wp.sh"]

CMD ["./webserv", "default.conf"]
