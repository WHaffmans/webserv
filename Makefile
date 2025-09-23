# Variables
# Detect if we're in a dev container or local development
ifeq ($(shell whoami),vscode)
    BUILD_DIR = build-container
else
    BUILD_DIR = build-local
endif

CMAKE = cmake
CMAKE_BUILD = cmake --build
CMAKE_FLAGS = -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
CONFIG_FILE = config/default.conf

# Default target
all: release

# Configure CMake if build directory doesn't exist
$(BUILD_DIR):
	$(CMAKE) -B $(BUILD_DIR) $(CMAKE_FLAGS)

# Build targets with specific build types
release: $(BUILD_DIR)
	$(CMAKE) -B $(BUILD_DIR) $(CMAKE_FLAGS) -DCMAKE_BUILD_TYPE=Release
	$(CMAKE_BUILD) $(BUILD_DIR) --target webserv --parallel

debug: $(BUILD_DIR)
	$(CMAKE) -B $(BUILD_DIR) $(CMAKE_FLAGS) -DCMAKE_BUILD_TYPE=Debug
	$(CMAKE_BUILD) $(BUILD_DIR) --target webserv

asan: $(BUILD_DIR)
	$(CMAKE) -B $(BUILD_DIR) $(CMAKE_FLAGS) -DCMAKE_BUILD_TYPE=ASAN
	$(CMAKE_BUILD) $(BUILD_DIR) --target webserv

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

# Mark targets as phony
.PHONY: all release debug asan run run_release run_debug run_asan clean fclean re
