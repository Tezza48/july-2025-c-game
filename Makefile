# --- Project Config ---
PROJECT := game
SRC_DIR    := src
ASSETS_DIR := assets
BUILD_DIR  := build
BIN_DIR    := dist

CC         := gcc
CFLAGS     := -w -Wextra -I$(SRC_DIR) -Isrc/vendor -Iinclude -g
LDFLAGS    := -lm -lopengl32 -lgdi32

# --- Cross-platform mkdir/copy ---
ifeq ($(OS),Windows_NT)
    MKDIR = if not exist "$(1)" mkdir "$(1)"
    COPY  = xcopy /E /Y /I "$(1)" "$(2)" >nul
else
    MKDIR = mkdir -p "$(1)"
    COPY  = cp -ru "$(1)" "$(2)"
endif

# --- Sources ---
SRC_FILES  := $(wildcard $(SRC_DIR)/*.c) \
              $(wildcard $(SRC_DIR)/vendor/*.c)
OBJ_FILES  := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))
TARGET     := $(BIN_DIR)/$(PROJECT)

ifeq ($(OS),Windows_NT)
TARGET := $(TARGET).exe
endif

# --- Rules ---
all: $(TARGET) copy_assets

$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(call MKDIR,$(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

copy_assets: | $(BIN_DIR)
	@echo Copying assets...
	$(call COPY,$(ASSETS_DIR)\*,$(BIN_DIR))

$(BIN_DIR) $(BUILD_DIR):
	$(call MKDIR,$@)

clean:
	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"
	@if exist "$(BIN_DIR)" rmdir /S /Q "$(BIN_DIR)"
	@if not exist "$(BUILD_DIR)" if not exist "$(BIN_DIR)" echo Cleaned.

run: all
	$(TARGET)

.PHONY: all clean run copy_assets
