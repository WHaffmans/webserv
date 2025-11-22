#!/bin/bash
# filepath: bisect_test.sh
# Git bisect run script for webserv
# Usage: git bisect start && git bisect bad && git bisect good <commit> && git bisect run ./bisect_test.sh

set -e

SERVER_PID=""
trap 'kill $SERVER_PID 2>/dev/null || true; sleep 1' EXIT INT TERM

echo "=== Testing $(git rev-parse --short HEAD) ==="

# Update submodules to match this commit
echo "Updating submodules..."
git submodule update --init --recursive || exit 125

# Build
make clean > /dev/null 2>&1 || true
make release > /dev/null 2>&1 || exit 125

# Start server
./build/webserv config/default.conf > /dev/null 2>&1 &
SERVER_PID=$!

# Wait for port
for i in {1..10}; do
    kill -0 $SERVER_PID 2>/dev/null || exit 125
    nc -z localhost 8083 2>/dev/null && break
    sleep 0.5
done

# Run tester
./ubuntu_tester http://localhost:8083/
TESTER_EXIT=$?

if [ -n "$SERVER_PID" ]; then
    # politely ask the server to stop
    kill -TERM "$SERVER_PID" 2>/dev/null || true

    # wait up to 5 seconds for it to exit
    for i in {1..10}; do
        kill -0 "$SERVER_PID" 2>/dev/null || break
        sleep 0.5
    done

    # force kill if still alive
    kill -KILL "$SERVER_PID" 2>/dev/null || true
    wait "$SERVER_PID" 2>/dev/null || true
fi

if [ $TESTER_EXIT -eq 0 ]; then
    echo "=== PASS ==="
    CODE=0
elif [ $TESTER_EXIT -eq 1 ]; then
    echo "=== FAIL ==="
    CODE=1
else
    echo "=== UNRESOLVED ==="
    CODE=125
fi

exit $CODE