#!/usr/bin/env python3
import cgi, sys, traceback

print("Content-Type: text/plain\r\n")

form = cgi.FieldStorage()
code = form.getvalue("code", "")

if not code.strip():
    print("ERROR: No code provided.")
    sys.exit(0)

try:
    # Create isolated execution space
    local_vars = {}
    exec(code, {}, local_vars)
    print("\n--- Execution Finished ---")

except Exception as e:
    print("ERROR:")
    print(traceback.format_exc())
