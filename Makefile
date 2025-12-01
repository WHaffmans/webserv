# Variables
BUILD_DIR = build
CMAKE = cmake
CMAKE_BUILD = cmake --build
CMAKE_FLAGS = -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
CONFIG_FILE = config/default.conf

# Environment detection
ifeq ($(shell whoami),vscode)
    CURRENT_ENV = container
else ifneq ($(REMOTE_CONTAINERS),)
    CURRENT_ENV = container
else
    CURRENT_ENV = local
endif

# Check if build directory was created in different environment
ifneq ($(wildcard $(BUILD_DIR)/.build-env),)
    PREVIOUS_ENV := $(shell cat $(BUILD_DIR)/.build-env 2>/dev/null || echo "unknown")
    ifneq ($(PREVIOUS_ENV),$(CURRENT_ENV))
        $(info Detected environment switch from $(PREVIOUS_ENV) to $(CURRENT_ENV) - cleaning build directory)
        $(shell rm -rf $(BUILD_DIR))
    endif
endif

# Default target
all: release

# Configure CMake if build directory doesn't exist
$(BUILD_DIR):
	$(CMAKE) -B $(BUILD_DIR) $(CMAKE_FLAGS)
	@echo "$(CURRENT_ENV)" > $(BUILD_DIR)/.build-env

# Build targets with specific build types
release: $(BUILD_DIR)
	$(CMAKE) -B $(BUILD_DIR) $(CMAKE_FLAGS) -DCMAKE_BUILD_TYPE=Release
	$(CMAKE_BUILD) $(BUILD_DIR) --target webserv --parallel
	@echo "$(CURRENT_ENV)" > $(BUILD_DIR)/.build-env

debug: $(BUILD_DIR)
	$(CMAKE) -B $(BUILD_DIR) $(CMAKE_FLAGS) -DCMAKE_BUILD_TYPE=Debug
	$(CMAKE_BUILD) $(BUILD_DIR) --target webserv --parallel
	@echo "$(CURRENT_ENV)" > $(BUILD_DIR)/.build-env

asan: $(BUILD_DIR)
	$(CMAKE) -B $(BUILD_DIR) $(CMAKE_FLAGS) -DCMAKE_BUILD_TYPE=ASAN
	$(CMAKE_BUILD) $(BUILD_DIR) --target webserv --parallel
	@echo "$(CURRENT_ENV)" > $(BUILD_DIR)/.build-env

run: run_release

# Run targets
run_release: release
	./$(BUILD_DIR)/webserv $(CONFIG_FILE)

run_debug: debug
	./$(BUILD_DIR)/webserv $(CONFIG_FILE)

run_asan: asan
	./$(BUILD_DIR)/webserv $(CONFIG_FILE)

# Clean targets
clean:
	@if [ -d "$(BUILD_DIR)" ]; then \
		$(CMAKE_BUILD) $(BUILD_DIR) --target clean; \
	fi

fclean:
	rm -rf $(BUILD_DIR)

# Rebuild everything
re: fclean all

# Test targets
test: release
	@echo "Building and running tests..."
	$(CMAKE_BUILD) $(BUILD_DIR) --target webserv_tests --parallel
	cd $(BUILD_DIR) && ctest --output-on-failure

test_verbose: release
	@echo "Building and running tests with verbose output..."
	$(CMAKE_BUILD) $(BUILD_DIR) --target webserv_tests --parallel
	cd $(BUILD_DIR) && ctest --verbose

test_build: release
	@echo "Building tests only..."
	$(CMAKE_BUILD) $(BUILD_DIR) --target webserv_tests --parallel

# Coverage targets
coverage:
	@echo "Running coverage analysis..."
	./scripts/coverage.sh

coverage_clean:
	@echo "Cleaning coverage data..."
	rm -rf build_coverage

# Manual coverage build (advanced users)
coverage_manual:
	@echo "Building with coverage manually..."
	@mkdir -p build_coverage
	cd build_coverage && $(CMAKE) .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DENABLE_COVERAGE=ON $(CMAKE_FLAGS)
	$(CMAKE_BUILD) build_coverage
	cd build_coverage && $(CMAKE_BUILD) . --target coverage

# Code formatting target
format:
	@echo "Running code formatting..."
	./scripts/format.sh

sources.mk: 
	@echo "Generating sources.mk..."
	./scripts/generate_sources.sh

dist: sources.mk
	@echo "Creating distribution package..."
	./scripts/create_dist.sh

# Mark targets as phony
.PHONY: all release debug asan run run_release run_debug run_asan clean fclean re test test_verbose test_build coverage coverage_clean coverage_manual format sources.mk