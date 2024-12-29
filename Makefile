# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -I/path/to/doctest $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -framework ApplicationServices  # Link SDL2 and Application Services framework for macOS

# Directories
SRC_DIR = src
CPU_DIR = $(SRC_DIR)/cpu
CYCLE_MGMT_DIR = $(CPU_DIR)/cpu_cycle_management
BUILD_DIR = build
TEST_DIR = tests

# Source files
SRCS = $(SRC_DIR)/main.cpp \
       $(CPU_DIR)/cpu.cpp \
       $(CPU_DIR)/cpu_arithmetic.cpp \
       $(CPU_DIR)/cpu_bitwise.cpp \
       $(CPU_DIR)/cpu_branch.cpp \
       $(CPU_DIR)/cpu_shift.cpp \
       $(CPU_DIR)/cpu_control.cpp \
       $(CPU_DIR)/cpu_misc.cpp \
       $(CPU_DIR)/cpu_flags.cpp \
       $(CPU_DIR)/cpu_stack.cpp \
       $(CPU_DIR)/cpu_memory.cpp \
       $(CPU_DIR)/cpu_transfer.cpp \
       $(CPU_DIR)/cpu_comparison.cpp \
       $(CYCLE_MGMT_DIR)/opcode_cycles.cpp \
       $(CYCLE_MGMT_DIR)/cycle_exceptions.cpp \
       $(SRC_DIR)/controller.cpp \
       $(SRC_DIR)/ppu.cpp

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(filter-out $(CPU_DIR)/%.cpp $(CYCLE_MGMT_DIR)/%.cpp, $(SRCS))) \
       $(patsubst $(CPU_DIR)/%.cpp, $(BUILD_DIR)/cpu_%.o, $(wildcard $(CPU_DIR)/*.cpp)) \
       $(patsubst $(CYCLE_MGMT_DIR)/%.cpp, $(BUILD_DIR)/cycle_mgmt_%.o, $(wildcard $(CYCLE_MGMT_DIR)/*.cpp))

# Test files
TEST_SRCS = $(TEST_DIR)/test_main.cpp \
            $(TEST_DIR)/test_cpu.cpp \
            $(TEST_DIR)/test_rti.cpp \
            $(TEST_DIR)/test_controller.cpp \
            $(TEST_DIR)/test_ppu.cpp  # PPU test file

TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))

# Target executable
TARGET = $(BUILD_DIR)/nes_emulator
TEST_TARGET = $(BUILD_DIR)/test_runner

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/cpu_%.o: $(CPU_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/cycle_mgmt_%.o: $(CYCLE_MGMT_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test rules
$(TEST_TARGET): $(TEST_OBJS) $(filter-out $(BUILD_DIR)/main.o, $(OBJS))  # Exclude main.o from test build
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/test_%.o: $(TEST_DIR)/test_%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run tests
tests: $(TEST_TARGET)
	$(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR) *.o
