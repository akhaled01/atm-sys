# Specify the directory for object files
OBJ_DIR := target

# List of object files
objects := $(addprefix $(OBJ_DIR)/, main.o system.o auth.o utils.o AccOps.o)

# Specify the target directory for the executable
TARGET_DIR := .

# Name of the executable
TARGET := $(TARGET_DIR)/atm

# Compiler flags
CFLAGS := -Isrc

# Default target
all: $(TARGET)

$(TARGET): $(objects)
	$(CC) -o $@ $^ -lmysqlclient -lncurses

# Rule for compiling object files
$(OBJ_DIR)/%.o: src/%.c src/header.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the object directory if it does not exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(objects)
	rmdir $(OBJ_DIR)
	rm -f $(TARGET)

.PHONY: all clean
