#!/usr/bin/env python3
import os, json, uuid, datetime
from http import cookies

print("Content-Type: text/plain")

# Directory for storing sessions
SESSION_DIR = "/tmp/sessions"
os.makedirs(SESSION_DIR, exist_ok=True)

raw_cookie = os.environ.get("HTTP_COOKIE", "")
incoming = cookies.SimpleCookie(raw_cookie)

# Check if SESSION_ID cookie already exists
session_id = incoming.get("SESSION_ID")

# Create a new session if needed
if session_id is None:
    new_id = str(uuid.uuid4())

    out = cookies.SimpleCookie()
    out["SESSION_ID"] = new_id
    out["SESSION_ID"]["path"] = "/"
    out["SESSION_ID"]["httponly"] = True
    print(out)

    session_file = os.path.join(SESSION_DIR, new_id + ".json")
    data = { "visits": 1, "created": str(datetime.datetime.now()), "last_visit": str(datetime.datetime.now()) }
    with open(session_file, "w") as f:
        json.dump(data, f)

    print()  # end headers
    print("=== New Session Created ===")
    print(f"Session ID: {new_id}")
    print(json.dumps(data, indent=2))
    exit()

# Load session data
session_id = session_id.value
session_file = os.path.join(SESSION_DIR, session_id + ".json")

if not os.path.exists(session_file):
    print()  # end headers
    print("ERROR: Session cookie exists but session file missing.")
    exit()

with open(session_file, "r") as f:
    data = json.load(f)

data["visits"] += 1
data["last_visit"] = str(datetime.datetime.now())

with open(session_file, "w") as f:
    json.dump(data, f)

print()  # end headers
print("=== Active Session ===")
print(f"Session ID: {session_id}")
print(json.dumps(data, indent=2))
