# =========================
# Compiler & flags
# =========================

CXX      := g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -Wpedantic
SIMD     := -mavx2
INCLUDES := -Iinclude

OPENCV_CFLAGS := $(shell pkg-config --cflags opencv4)
OPENCV_LIBS   := $(shell pkg-config --libs opencv4)

# =========================
# Project structure
# =========================

SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin

TARGET   := conv2d.run

SOURCES  := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/conv2d_baseline.cpp \
	$(SRC_DIR)/utility.cpp \
	$(SRC_DIR)/io.cpp

OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# =========================
# Default target
# =========================

all: $(BIN_DIR)/$(TARGET)

# =========================
# Link
# =========================

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SIMD) $^ -o $@ $(OPENCV_LIBS)

# =========================
# Compile
# =========================

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(SIMD) $(INCLUDES) $(OPENCV_CFLAGS) -c $< -o $@

# =========================
# Utility targets
# =========================

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

rebuild: clean all

.PHONY: all clean rebuild
