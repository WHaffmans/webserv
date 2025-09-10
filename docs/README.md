# webserv


### Date Flow Overview

```
src/
 ├─ main.cpp              # starts server
 ├─ Server.cpp/.hpp       # listens & manages event loop
 ├─ Connection.cpp/.hpp   # represents a single client
 ├─ RequestParser.cpp/.hpp
 ├─ Router.cpp/.hpp
 ├─ Response.cpp/.hpp
 ├─ CGIHandler.cpp/.hpp
 ├─ Config.cpp/.hpp       # parses config file
 └─ Utils.cpp/.hpp
```

```
[Client TCP packet]
       │
       ▼
┌───────────────┐
│ Event Loop    │  (poll/select/epoll)
└──────┬────────┘
       │
       ▼
┌───────────────┐
│ RequestParser │  (builds Request struct)
└──────┬────────┘
       │
       ▼
┌───────────────┐
│ Router        │  (selects location, checks methods)
└──────┬────────┘
       │
       ▼
┌───────────────┐
│ Handler       │  (static file, CGI, error)
└──────┬────────┘
       │
       ▼
┌───────────────┐
│ ResponseBuilder│ (status line + headers + body)
└──────┬────────┘
       │
       ▼
[Send response back to client]
```