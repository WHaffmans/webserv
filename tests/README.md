# Webserv Unit Test Framework

This directory contains the unit test suite for the webserv project, built using Google Test (gtest).

## Structure

```
tests/
├── CMakeLists.txt          # CMake configuration for tests
├── test_main.cpp           # Main test runner
├── config/                 # Configuration system tests
│   └── test_directives.cpp # Directive classes tests
├── http/                   # HTTP handling tests
│   └── test_http_headers.cpp # HttpHeaders class tests
├── log/                    # Logging system tests
│   └── test_log.cpp        # Log class tests
└── socket/                 # Socket handling tests
    └── test_socket.cpp     # Socket class tests
```

## Running Tests

### Quick Commands

```bash
# Build and run all tests
make test

# Run tests with verbose output
make test_verbose

# Only build tests (don't run)
make test_build
```

### Manual CMake Commands

```bash
# Configure and build
mkdir build && cd build
cmake ..
make webserv_tests

# Run tests
ctest --output-on-failure

# Run tests with verbose output
ctest --verbose
```

## Writing New Tests

### 1. Create a New Test File

Create your test file in the appropriate subdirectory:

```cpp
#include <gtest/gtest.h>
#include <webserv/your/header.hpp>

class YourClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }
    
    void TearDown() override {
        // Cleanup code
    }

public:
    // Test data members
};

TEST_F(YourClassTest, TestName) {
    // Your test code here
    EXPECT_EQ(expected, actual);
    ASSERT_TRUE(condition);
}
```

### 2. Common Google Test Assertions

- `EXPECT_EQ(expected, actual)` - Values are equal
- `EXPECT_NE(val1, val2)` - Values are not equal  
- `EXPECT_TRUE(condition)` - Condition is true
- `EXPECT_FALSE(condition)` - Condition is false
- `EXPECT_STREQ(str1, str2)` - C strings are equal
- `ASSERT_*` variants - Same as EXPECT but stop test on failure

### 3. Test Organization

- Put tests for a class in `test_classname.cpp`
- Group related tests in test fixtures (classes inheriting from `::testing::Test`)
- Use descriptive test names: `TEST_F(ClassName, DescriptiveTestName)`

## Test Categories

### Unit Tests
- Test individual classes and functions in isolation
- Mock external dependencies when necessary
- Fast execution, no network/file system dependencies

### Integration Tests
- Test interaction between components
- May involve actual network sockets, file operations
- Longer execution time acceptable

## Current Test Coverage

- **HttpHeaders**: Header management, parsing, case-insensitive operations
- **Directives**: String, Int, Bool directive creation and parsing
- **Logging**: Log level conversions, color codes, basic functionality  
- **Socket**: Basic construction and move semantics

## Adding Dependencies

If you need additional test utilities:

1. Add them to the CMake FetchContent in the main CMakeLists.txt
2. Link them in tests/CMakeLists.txt
3. Include them in your test files

## Best Practices

1. **Test one thing at a time** - Each test should verify a single behavior
2. **Use descriptive names** - Test names should clearly indicate what they verify
3. **Arrange-Act-Assert** - Structure tests with setup, action, and verification
4. **Test edge cases** - Empty inputs, null pointers, boundary values
5. **Keep tests independent** - Tests should not depend on each other
6. **Mock external dependencies** - Isolate the code under test

## Continuous Integration

Tests are automatically built and run as part of the CI pipeline. All tests must pass before code can be merged.

## Troubleshooting

### Test Build Failures
- Check that all includes are correct
- Ensure the main library builds successfully first
- Verify CMake configuration

### Test Runtime Failures  
- Use `make test_verbose` for detailed output
- Check test setup/teardown code
- Verify test assertions and expected values

### Performance Issues
- Keep unit tests fast (< 1ms each typically)
- Use mocks for expensive operations
- Consider moving slow tests to integration test suite