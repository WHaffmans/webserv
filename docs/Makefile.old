# Compiler and flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -MMD -MP

# Build type (default to debug)
BUILD_TYPE ?= debug

# Build-specific flags
ifeq ($(BUILD_TYPE), release)
	CXXFLAGS += -O3 -DNDEBUG
	BUILD_SUFFIX = release
else ifeq ($(BUILD_TYPE), asan)
	CXXFLAGS += -g -O1 -fsanitize=address -fno-omit-frame-pointer
	BUILD_SUFFIX = asan
else
	CXXFLAGS += -g -O0 -DDEBUG
	BUILD_SUFFIX = debug
endif

# Directories
SRCDIR = webserv
BUILDDIR = build/$(BUILD_SUFFIX)
OBJDIR = $(BUILDDIR)/obj

# Target executable
NAME = $(BUILDDIR)/webserv

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp) \
          $(wildcard $(SRCDIR)/*/*.cpp)

# Object files
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Dependency files
DEPENDS = $(OBJECTS:.o=.d)

# Include directories
INCLUDES = -I.

# Default target
all: $(NAME)

# Create executable
$(NAME): $(OBJECTS)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(NAME)

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp Makefile
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean object files
clean:
	rm -rf $(OBJDIR)

# Clean everything (object files, executable, and build directory)
fclean: clean
	rm -rf $(BUILDDIR)

# Clean all build types
fclean-all:
	rm -rf build

# Rebuild everything
re: fclean all

# Run the executable (rebuild first)
run: re
	./$(NAME)

# Build type targets
debug:
	$(MAKE) BUILD_TYPE=debug

release:
	$(MAKE) BUILD_TYPE=release

asan:
	$(MAKE) BUILD_TYPE=asan

# Phony targets
.PHONY: all clean fclean fclean-all re run debug release asan

# Include dependency files
-include $(DEPENDS)