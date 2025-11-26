#pragma once

#include <cstddef>

#define LOG_LEVEL_DEFINE Info

#define CGI_TIMEOUT 30

#define DEFAULT_TIMEOUT 30

namespace Constants
{
constexpr static size_t BUFFER_SIZE = 8192; // 8kb
constexpr static size_t CHUNK_SIZE = 65536; // 64kb
} // namespace Constants