# ================================
# Portable Makefile (Linux / Windows MSYS2)
# ================================

# Tools
CXX      ?= g++

# Project structure
SRC_DIR   := source
BUILD_DIR := build

# Sources and objects
SRCS      := $(wildcard $(SRC_DIR)/*.cpp)
OBJS      := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

MKDIR_P := mkdir -p $(BUILD_DIR)
CLEAN_CMD := rm -rf $(BUILD_DIR)

# Detect OS
ifeq ($(OS),Windows_NT)
    EXEEXT   := .exe
else
    EXEEXT   :=
endif

# Output binary
TARGET    := $(BUILD_DIR)/scanner$(EXEEXT)

# Flags
CXXFLAGS ?= -g -std=c++17 -Wall -Wextra -MMD -MP
CXXFLAGS += $(shell pkg-config --cflags openssl)

LDFLAGS  ?= -static
LDFLAGS  += $(shell pkg-config --libs openssl)

# ================================
# Rules
# ================================

all: $(TARGET)

# Link
$(TARGET): $(OBJS)
		$(MKDIR_P)
		$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
		$(MKDIR_P)
		$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
		$(CLEAN_CMD)

# Dependencies
-include $(OBJS:.o=.d)

.PHONY: all clean
