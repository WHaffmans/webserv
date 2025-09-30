#!/bin/bash

./check_iwyu.sh
./fix_iwyu_auto.sh

find webserv -name "*.hpp" -o -name "*.cpp" | xargs sed -i -E 's/#include "(.*)"/#include <\1>/g'
find webserv -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
find webserv -name "*.hpp" -exec sh -c 'if ! grep -q "#pragma once" "$1"; then echo "Missing #pragma once in $1"; fi' _ {} \;