CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LIBS = -lncurses -ltinfo
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/editor.c $(SRC_DIR)/display.c $(SRC_DIR)/input.c $(SRC_DIR)/syntax.c $(SRC_DIR)/file_explorer.c $(SRC_DIR)/modes.c $(SRC_DIR)/settings.c $(SRC_DIR)/plugin.c $(SRC_DIR)/buffer.c

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Target executable
TARGET = vicode

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link object files to create executable
$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Install the editor
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall the editor
uninstall:
	rm -f /usr/local/bin/$(TARGET)

.PHONY: all clean install uninstall