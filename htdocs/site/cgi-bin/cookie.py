#!/usr/bin/env python3
import cgi, os
from http import cookies
import datetime

print("Content-Type: text/plain")

form = cgi.FieldStorage()
set_value = form.getvalue("set")
clear = form.getvalue("clear")

# Parse incoming cookies
raw_cookie = os.environ.get("HTTP_COOKIE", "")
cookie = cookies.SimpleCookie(raw_cookie)

# Handle setting a cookie
if set_value is not None:
    out = cookies.SimpleCookie()
    out["demo"] = set_value
    out["demo"]["path"] = "/"
    out["demo"]["httponly"] = True

    print(out)  # prints Set-Cookie header automatically
    print()     # end headers
    print("Cookie set: demo=", set_value)
    exit()

# Handle clearing the cookie
if clear is not None:
    out = cookies.SimpleCookie()
    out["demo"] = ""
    out["demo"]["path"] = "/"
    out["demo"]["expires"] = "Thu, 01 Jan 1970 00:00:00 GMT"

    print(out)
    print()
    print("Cookie cleared")
    exit()

print()  # end headers

print("=== Incoming Cookies ===")
if cookie:
    for key in cookie:
        print(f"{key} = {cookie[key].value}")
else:
    print("(No cookies sent)")
