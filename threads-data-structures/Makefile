# Compiler and flags
CC := gcc
CFLAGS := -Wall -g

# Directories
BUILD_DIR := build

# Files
SRCS := $(filter-out test_%.c, $(wildcard *.c))
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET := main

# Default target
all: $(TARGET)

# Link objects into binary
$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Compile source files into build directory
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

TESTS := test_approxcounter
TEST_BINS := $(patsubst %,$(BUILD_DIR)/%,$(TESTS))

test: $(TEST_BINS)
	@for t in $(TEST_BINS); do \
		echo "Running $$t..."; \
		$$t || exit 1; \
	done
	@echo "All tests passed."

$(BUILD_DIR)/test_%: test_%.c %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@
