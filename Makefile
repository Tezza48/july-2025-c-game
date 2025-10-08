# Directories
SRC_DIR := src
VENDOR_DIR := vendor
INCLUDE_DIR := include
DIST_DIR := dist
SHADERS_DIR := shaders

# Compiler and flags
CC := gcc
CFLAGS := -w -O0 -g -I$(INCLUDE_DIR)
LDFLAGS := -lopengl32 -lgdi32
TARGET := $(DIST_DIR)/game.exe

# Source and object files
SRC := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(VENDOR_DIR)/*.c)
OBJ := $(patsubst %.c, %.o, $(SRC))

.PHONY: all clean dist copy_shaders

all: $(TARGET) copy_shaders

# Ensure dist directory exists
dist:
	if not exist $(DIST_DIR) mkdir $(DIST_DIR)

# Copy shader files
SHADERS := $(wildcard $(SHADERS_DIR)/*.vert $(SHADERS_DIR)/*.frag)
COPIED  := $(patsubst $(SHADERS_DIR)/%, $(DIST_DIR)/%, $(SHADERS))

$(DIST_DIR)/%.vert $(DIST_DIR)/%.frag: $(SHADERS_DIR)/% | $(DIST_DIR)
	@cp $< $@

copy_shaders: $(COPIED)

# Build executable
$(TARGET): $(OBJ) | dist
	$(CC) $(OBJ) $(LDFLAGS) -o $@

# Compile C source files into objects
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(SRC_DIR)/*.o $(VENDOR_DIR)/*.o $(TARGET)
	rm -f $(DIST_DIR)/*.vert $(DIST_DIR)/*.frag