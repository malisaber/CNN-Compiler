CXX ?= g++
TARGET ?= build/CNN-Compiler

CLI11_HEADER := include/CLI11.hpp
JSON_HEADER := include/json.hpp
CLI11_URL ?= https://github.com/CLIUtils/CLI11/releases/download/v2.6.2/CLI11.hpp
JSON_URL ?= https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp

SRC_DIR := src
INC_DIR := include
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -I$(INC_DIR)
LDFLAGS ?=
LDLIBS ?=

.PHONY: all clean run deps check-tools cli11 json

all: $(TARGET)

deps: check-tools cli11 json

check-tools:
	@command -v "$(CXX)" >/dev/null 2>&1 || { printf '%s\n' "Missing C++ compiler: $(CXX)" >&2; exit 1; }
	@command -v curl >/dev/null 2>&1 || { printf '%s\n' "Missing dependency installer: curl" >&2; exit 1; }

cli11:
	@if [ ! -s "$(CLI11_HEADER)" ] || grep -q '^404: Not Found$$' "$(CLI11_HEADER)"; then \
		mkdir -p $(dir $(CLI11_HEADER)); \
		curl -fsSL "$(CLI11_URL)" -o "$(CLI11_HEADER)"; \
		printf '%s\n' "Fetched CLI11.hpp"; \
	fi

json:
	@if [ ! -s "$(JSON_HEADER)" ] || grep -q '^404: Not Found$$' "$(JSON_HEADER)"; then \
		mkdir -p $(dir $(JSON_HEADER)); \
		curl -fsSL "$(JSON_URL)" -o "$(JSON_HEADER)"; \
		printf '%s\n' "Fetched json.hpp"; \
	fi

$(CLI11_HEADER):
	@mkdir -p $(dir $@)
	@curl -fsSL "$(CLI11_URL)" -o "$@"
	@printf '%s\n' "Fetched CLI11.hpp"

$(JSON_HEADER):
	@mkdir -p $(dir $@)
	@curl -fsSL "$(JSON_URL)" -o "$@"
	@printf '%s\n' "Fetched json.hpp"

$(TARGET): $(OBJS) | deps $(BUILD_DIR)
	$(CXX) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(CLI11_HEADER) $(JSON_HEADER) | deps $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

-include $(DEPS)
