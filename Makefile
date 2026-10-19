CC = gcc
LDFLAGS = -lsqlite3 -lm

# Prefer pkg-config for libraries that are not in the default search path
# (e.g. Homebrew on Apple Silicon installs to /opt/homebrew/include).
# Fall back to plain linker flags when pkg-config or the .pc file is absent.
YAML_CFLAGS := $(shell pkg-config --cflags yaml-0.1 2>/dev/null)
YAML_LIBS   := $(shell pkg-config --libs   yaml-0.1 2>/dev/null)
CJSON_CFLAGS := $(shell pkg-config --cflags libcjson 2>/dev/null)
CJSON_LIBS   := $(shell pkg-config --libs   libcjson 2>/dev/null)

CFLAGS = -Iinclude $(YAML_CFLAGS) $(CJSON_CFLAGS)
ifeq ($(strip $(YAML_LIBS)),)
	YAML_LIBS = -lyaml
endif
ifeq ($(strip $(CJSON_LIBS)),)
	CJSON_LIBS = -lcjson
endif
LDFLAGS += $(YAML_LIBS) $(CJSON_LIBS)

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

ifdef SANITIZE
	CFLAGS += -fsanitize=address,undefined
	LDFLAGS += -fsanitize=address,undefined
endif

# -Wall applies to every build; RELEASE only controls optimisation/debug info
CFLAGS += -Wall -Wextra
ifndef RELEASE
	CFLAGS += -DDEBUG -ggdb
else
	CFLAGS += -O3
endif

# List all the source files
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')

# Generate the corresponding object file names
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))
DEP_FILES = $(OBJ_FILES:.o=.d)

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

# Rule to build object files from source files.
# -MMD -MP records header dependencies so editing a header rebuilds its objects.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

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
.PHONY: all clean install autocomplete compile_commands.json

-include $(DEP_FILES)
