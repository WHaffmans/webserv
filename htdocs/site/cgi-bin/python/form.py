#!/usr/bin/env python3
import cgi
import cgitb
cgitb.enable()

print("Content-Type: text/plain\r\n")

form = cgi.FieldStorage()

name = form.getvalue("name", "(empty)")
email = form.getvalue("email", "(empty)")
message = form.getvalue("message", "(empty)")

print("=== Form Data Received ===")
print(f"Name: {name}")
print(f"Email: {email}")
print(f"Message: {message}")
print()
print("=== Validation ===")
print(f"Name valid: {'Yes' if len(name)>=2 else 'No'}")
print(f"Email valid: {'Yes' if '@' in email else 'No'}")
print(f"Message length: {len(message)}")
print()
print("Processed by Python CGI")
