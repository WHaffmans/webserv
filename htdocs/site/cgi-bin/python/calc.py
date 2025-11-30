#!/usr/bin/env python3
import cgi
import math

print("Content-Type: text/plain\r\n")

form = cgi.FieldStorage()
a = float(form.getvalue("a", 0))
b = float(form.getvalue("b", 0))
op = form.getvalue("op", "add")

def calc(a, b, op):
    if op == "add": return a + b
    if op == "subtract": return a - b
    if op == "multiply": return a * b
    if op == "divide":
        if b == 0: return "ERROR: Division by zero"
        return a / b
    if op == "power": return a ** b
    if op == "modulo":
        if b == 0: return "ERROR: Modulo by zero"
        return a % b
    return "Unknown operation"

result = calc(a, b, op)
print(result)
