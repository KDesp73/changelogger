CC = gcc
CFLAGS = -Iinclude
LDFLAGS = -lsqlite3 -lyaml -lcjson -lm

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

ifdef SANITIZE
	CFLAGS += -fsanitize=address,undefined
	LDFLAGS += -fsanitize=address,undefined
endif

ifndef RELEASE
	CFLAGS += -DDEBUG -ggdb -Wall
else 
	CFLAGS += -O3
endif

# List all the source files
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')

# Generate the corresponding object file names
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Target: the final executable
TARGET = changelogger

# Default target, build the executable
all: $(BUILD_DIR) $(TARGET)

# Rule to create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/commands

# Rule to build the executable
$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

# Rule to build object files from source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean rule to remove generated files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

compile_commands.json: $(SRC_FILES)
	bear -- make

install:
	./config.sh install

autocomplete:
	complgen aot ./docs/changelogger.usage --zsh-script ./docs/_changelogger.zsh
	complgen aot ./docs/changelogger.usage --bash-script ./docs/_changelogger.bash
	complgen aot ./docs/changelogger.usage --fish-script ./docs/_changelogger.fish

# Phony target to avoid conflicts with file names
.PHONY: all clean

