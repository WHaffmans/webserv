find webserv -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
find webserv -name "*.hpp" -exec sh -c 'if ! grep -q "#pragma once" "$1"; then echo "Missing #pragma once in $1"; fi' _ {} \;