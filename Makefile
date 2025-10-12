# Directories
SRC_DIR := src
VENDOR_DIR := vendor
INCLUDE_DIR := include
DIST_DIR := dist
SHADERS_DIR := shaders
ASSETS_DIR := assets

# Compiler and flags
CC := gcc
CFLAGS := -w -O0 -g -I$(INCLUDE_DIR)
LDFLAGS := -lopengl32 -lgdi32
TARGET := $(DIST_DIR)/game.exe

# Source and object files
SRC := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(VENDOR_DIR)/*.c)
OBJ := $(patsubst %.c, %.o, $(SRC))

.PHONY: all clean dist copy_shaders copy_assets

all: $(TARGET) copy_shaders copy_assets

# Ensure dist directory exists (cross-platform)
dist:
	@mkdir -p "$(DIST_DIR)" 2>/dev/null || if not exist "$(DIST_DIR)" mkdir "$(DIST_DIR)"

# --- Shaders ---
SHADERS := $(wildcard $(SHADERS_DIR)/*.vert $(SHADERS_DIR)/*.frag)
COPIED  := $(patsubst $(SHADERS_DIR)/%, $(DIST_DIR)/%, $(SHADERS))

$(DIST_DIR)/%.vert: $(SHADERS_DIR)/%.vert | $(DIST_DIR)
	@cp $< $@

$(DIST_DIR)/%.frag: $(SHADERS_DIR)/%.frag | $(DIST_DIR)
	@cp $< $@

copy_shaders: $(COPIED)

# --- Assets ---
ASSETS_SRC := $(shell dir /B /S $(ASSETS_DIR) 2>NUL)
ASSETS_DST := $(patsubst $(ASSETS_DIR)/%, $(DIST_DIR)/assets/%, $(ASSETS_SRC))

$(DIST_DIR)/assets/%: $(ASSETS_DIR)/% | dist/assets
	@mkdir -p "$(dir $@)" 2>/dev/null || if not exist "$(dir $@)" mkdir "$(dir $@)"
	@cp "$<" "$@" 2>/dev/null || copy /Y "$<" "$@"

dist/assets:
	@mkdir -p "$@" 2>/dev/null || if not exist "$@" mkdir "$@"

copy_assets: $(ASSETS_DST)

# --- Build executable ---
$(TARGET): $(OBJ) | dist
	$(CC) $(OBJ) $(LDFLAGS) -o "$@"

# --- Compile source files ---
%.o: %.c
	$(CC) $(CFLAGS) -c "$<" -o "$@"

# --- Clean ---
clean:
	@rm -f $(SRC_DIR)/*.o $(VENDOR_DIR)/*.o $(TARGET) 2>/dev/null || del /Q $(SRC_DIR)\*.o $(VENDOR_DIR)\*.o "$(TARGET)" 2>nul
	@rm -f $(DIST_DIR)/*.vert $(DIST_DIR)/*.frag 2>/dev/null || del /Q $(DIST_DIR)\*.vert $(DIST_DIR)\*.frag 2>nul
