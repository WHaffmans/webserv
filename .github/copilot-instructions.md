# Webserv - AI Coding Agent Instructions

## Project Overview
A C++20 HTTP/1.1 web server implementing epoll-based event-driven architecture. Core components: configuration parser, HTTP request/response handling, CGI execution, static file serving, and routing.

## Architecture Fundamentals

### Event Loop & Request Flow
```
Client → epoll_wait → Server::handleEvent → Client → Router → Handler → Response
```

**Critical pattern**: The server uses a single epoll instance (`Server::epoll_fd_`) to multiplex I/O:
1. `Server::run()` contains the main event loop calling `epoll_wait()` with 10ms timeout
2. Events trigger specific handlers: `EPOLLIN` → request reading, `EPOLLOUT` → response writing
3. Each `Client` manages its own sockets and handler state machines
4. Sockets transition through states tracked in `ASocket::IoState` (READ/WRITE)

**Why this matters**: All I/O is non-blocking. Never call blocking operations. Use `socket->setIOState()` and `server.update(socket)` to change epoll interest masks.

### Configuration System
Three-tier hierarchy: `GlobalConfig` → `ServerConfig` → `LocationConfig`

**Directive resolution**: Uses inheritance with `AConfig::get<T>(name)` - searches current config, falls back to parent. Example:
```cpp
auto maxBodySize = locationConfig->get<size_t>("client_max_body_size")
    .value_or(serverConfig->get<size_t>("client_max_body_size").value_or(1048576));
```

**Validation architecture**: Two-stage validation in `ConfigValidator`:
1. **Structural rules** (`AStructuralValidationRule`): Check block-level requirements (e.g., `RequiredDirectivesRule`)
2. **Directive rules** (`AValidationRule`): Validate individual directive values (e.g., `PortValidationRule`)

Rules are registered in `ConfigValidator` constructor and executed by `ValidationEngine`.

**Context-aware directives**: The `DirectiveFactory` uses a context string (`"GSL"` = Global/Server/Location) to restrict where directives can appear. Check `DirectiveFactory::supportedDirectives` when adding new directives.

### CGI Execution Pipeline
**Process model**: `fork()` → `pipe2()` for stdin/stdout/stderr → `execve()` in child

**Critical implementation details**:
1. Use `pipe2(O_CLOEXEC | O_NONBLOCK)` - flags prevent fd leaks and blocking
2. Child process: `dup2()` pipes to std streams, call `Log::clearChannels()` before `execve()`
3. Parent: Wrap pipe fds in `CgiSocket` objects, register with `Client::addSocket()`
4. Environment: `CgiEnvironment` class builds CGI/1.1 compliant env vars (required: `GATEWAY_INTERFACE`, `SERVER_PROTOCOL`, `REQUEST_METHOD`, etc.)
5. Timeout handling: `TimerSocket` with `timerfd_create()` registered in epoll

**State machine**: CgiHandler writes request body → reads response headers → parses headers → reads body → `waitpid(WNOHANG)` to check status.

### HTTP Request Parsing
State machine in `HttpRequest::State`: `RequestLine → Headers → Body/Chunked → Complete/ParseError`

**Chunked transfer encoding**: Implemented in `parseBufferforChunkedBody()`:
- Read chunk size (hex) → validate → read chunk data → repeat until size=0
- Parse errors set `State::ParseError` and call `response.setError(400)`

**Critical validation**: Host header is mandatory (HTTP/1.1). Checked in `setState(State::Complete)`.

## Build & Test System

### Build Configuration
- **CMake build types**: `Release` (default), `Debug`, `ASAN` (AddressSanitizer)
- **Makefile wrapper**: `make release/debug/asan` builds specific configurations
- **Environment detection**: Makefile tracks container vs local builds in `build/.build-env`, auto-cleans on switch

### Test Commands
```bash
make test              # Build + run unit tests (Google Test)
make test_verbose      # Run with detailed output
make coverage          # Generate coverage report (requires lcov or gcovr)
./webserv-tester/bin/run_tests.py [--suite SUITE] [--test TEST]
```

**Test structure**:
- Unit tests: `tests/` directory, organized by component
- Integration tests: `webserv-tester/` Python test framework
- Test config: `webserv-tester/data/conf/test.conf` (port 8080)

### Integration Testing with webserv-tester

The `webserv-tester/` directory contains a comprehensive Python-based integration test framework that validates HTTP/1.1 compliance, configuration handling, and feature implementation.

**Running the tester**:
```bash
# Run all tests (automatically starts/stops server)
./run_test.sh

# Run specific test suite(s)
./run_test.sh basic
./run_test.sh http
./run_test.sh cgi

```

**Available test suites** (in `webserv-tester/tests_suites/`):
- `basic` (`basic_tests.py`): Smoke tests for fundamental functionality (server start, static files, basic requests)
- `http` (`http_tests.py`): HTTP/1.1 protocol compliance (headers, status codes, chunked encoding, keep-alive, malformed requests)
- `cgi` (`cgi_tests.py`): CGI/1.1 execution (environment variables, stdin/stdout handling, timeouts, error handling)
- `method` (`method_tests.py`): HTTP method support per location (GET, POST, DELETE validation against config)
- `config` (`config_tests.py`): Configuration directives (inheritance, root, index, autoindex, error pages, redirects, location matching)
- `invalid` (`invalid_config_tests.py`): Error handling for malformed configs (missing directives, invalid contexts, syntax errors)
- `upload` (`upload_tests.py`): File upload functionality
- `uri` (`uri_tests.py`): URI parsing and handling
- `redirect` (`redirect_tests.py`): HTTP redirect handling
- `cookie` (`cookie_tests.py`): Cookie handling
- `security` (`security_tests.py`): Security-related tests
- `performance` (`performance_tests.py`): Performance benchmarks

**Test framework architecture**:
- `core/test_case.py`: Base class for all tests with assertion helpers
- `core/server_manager.py`: Manages server process lifecycle (start/stop/restart)
- `core/test_runner.py`: HTTP request utilities and response validation
- `data/conf/test.conf`: Test server configuration (port 8080, multiple locations)
- `data/www/`: Test web content (HTML, CGI scripts, static files)

**Writing new tests**: Tests inherit from `TestCase` class and follow this pattern:
```python
class MyTests(TestCase):
    def test_my_feature(self):
        response = self.runner.send_request('GET', '/path')
        self.assert_equals(response.status_code, 200, "Expected 200 OK")
        self.assert_true('Content-Type' in response.headers, "Missing header")
```

Find test source in `webserv-tester/tests_suites/` to understand test scenarios or add new tests for your features.

## Code Conventions

### Include Order (enforced by .clang-format)
1. Own header (`"Class.hpp"`)
2. Project headers (`<webserv/path/Header.hpp>`)
3. C++ standard library (`<string>`)
4. C headers (`<unistd.h>`)

### Logging Pattern
Use `Log::trace(LOCATION)` at function entry for debugging. Available levels: `trace`, `debug`, `info`, `warning`, `error`, `fatal`.

**Important**: Always log before throwing exceptions or returning errors.

### Error Handling
- **HTTP errors**: Call `ErrorHandler::createErrorResponse(statusCode, response, config)` - handles custom error pages
- **Validation errors**: Throw `RequestValidator::ValidationException{statusCode}` in Router
- **Config errors**: Throw `std::runtime_error` with descriptive message during parsing
- **CGI errors**: Check `cgiProcess_->getExitCode()`, set `response.setStatus(500)` if non-zero

### Memory Management
- Use `std::unique_ptr` for ownership (e.g., `Client` owns `ClientSocket`)
- Pass raw pointers for non-owning references (e.g., `Server&` in `Client`)
- **Socket ownership**: `Server` owns `ServerSocket`, `Client` owns `ClientSocket` and `CgiSocket`

## Common Patterns & Gotchas

### Adding a New Handler
1. Inherit from `AHandler`, implement `handle()` and `handleTimeout()`
2. Register in `Router::handleRequest()` based on URI properties
3. Use `startTimer()` from base class if operation may block
4. Set response complete: `response_.setComplete()`

### Adding a Configuration Directive
1. Add to `DirectiveFactory::supportedDirectives` with context string
2. Create validation rule implementing `AValidationRule`
3. Register in `ConfigValidator` constructor: `engine_->addServerRule(name, std::make_unique<Rule>())`
4. Access in code: `config->get<Type>("directive_name")`

### Socket State Management
**Critical**: After modifying socket interest (read→write or vice versa):
```cpp
socket->setIOState(ASocket::IoState::WRITE);
socket->markDirty();  // Flags for epoll update
// Server polls dirty sockets in pollSockets() and calls update()
```

### URI Resolution
`URI` class handles path resolution:
- `matchConfig()`: Longest prefix match for location blocks
- `getFullPath()`: Resolves root + location path + request path
- `isCgi()`: Checks if path matches `cgi_ext` directive
- `isRedirect()`: Checks for redirect directive

## Testing Best Practices

### Unit Test Structure
Follow GTest patterns in `tests/`:
- Use test fixtures inheriting from `::testing::Test`
- Name tests descriptively: `TEST_F(ClassTest, MethodName_Scenario_ExpectedBehavior)`
- One assertion per logical check
- Mock external dependencies (sockets, file I/O)

### Integration Test Organization
`webserv-tester/tests_suites/` contains:
- `basic_tests.py`: Smoke tests (server start, static files)
- `http_tests.py`: Protocol compliance (headers, status codes, chunked encoding)
- `cgi_tests.py`: CGI execution and environment variables
- `method_tests.py`: HTTP method support per location
- `config_tests.py`: Directive inheritance and validation
- `invalid_config_tests.py`: Error handling for malformed configs

## Key Files Reference

- `webserv/main.cpp`: Entry point, signal handling
- `webserv/server/Server.{hpp,cpp}`: Event loop, epoll management
- `webserv/client/Client.{hpp,cpp}`: Per-connection state
- `webserv/config/ConfigManager.hpp`: Singleton config access
- `webserv/config/validation/ConfigValidator.cpp`: Validation rule registration
- `webserv/router/Router.cpp`: Request routing logic
- `webserv/handler/CgiProcess.cpp`: fork/exec implementation
- `webserv/http/HttpRequest.cpp`: State machine for parsing
- `CMakeLists.txt`: Build configuration and test setup

## Debugging Tips

### AddressSanitizer Build
```bash
make asan
./build/webserv config/default.conf
```
Use for memory leaks, use-after-free, double-free detection.

### CGI Debugging
CGI child process stderr goes to `CgiSocket` read in `CgiHandler::error()`. Check logs for script output.

### Epoll Issues
Enable trace logging: modify `Log::setLevel(Log::Level::TRACE)` in `main.cpp`. Watch for socket fd lifecycle in logs.

### Config Validation
Run `ConfigValidator` checks before starting server. Errors print to stderr with context (global/server/location and directive name).
