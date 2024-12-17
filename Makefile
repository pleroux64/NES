# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -I/path/to/doctest $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -framework ApplicationServices  # Link SDL2 and Application Services framework for macOS

# Directories
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

# Source files
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/cpu.cpp \
       $(SRC_DIR)/cpu_arithmetic.cpp \
       $(SRC_DIR)/cpu_bitwise.cpp \
       $(SRC_DIR)/cpu_branch.cpp \
       $(SRC_DIR)/cpu_shift.cpp \
       $(SRC_DIR)/cpu_control.cpp \
       $(SRC_DIR)/cpu_misc.cpp \
       $(SRC_DIR)/cpu_flags.cpp \
       $(SRC_DIR)/cpu_stack.cpp \
       $(SRC_DIR)/cpu_memory.cpp \
       $(SRC_DIR)/cpu_transfer.cpp \
       $(SRC_DIR)/cpu_comparison.cpp \
       $(SRC_DIR)/opcode_cycles.cpp \
       $(SRC_DIR)/cycle_exceptions.cpp \
       $(SRC_DIR)/controller.cpp \
       $(SRC_DIR)/ppu.cpp  # Added PPU source file

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Test files
TEST_SRCS = $(TEST_DIR)/test_main.cpp \
            $(TEST_DIR)/test_cpu.cpp \
            $(TEST_DIR)/test_rti.cpp \
            $(TEST_DIR)/test_controller.cpp \
            $(TEST_DIR)/test_ppu.cpp  # Added PPU test file

TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))

# Target executable
TARGET = $(BUILD_DIR)/nes_emulator
TEST_TARGET = $(BUILD_DIR)/test_runner

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test rules
$(TEST_TARGET): $(TEST_OBJS) $(filter-out $(BUILD_DIR)/main.o, $(OBJS))  # Exclude main.o from test build
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/test_main.o: $(TEST_DIR)/test_main.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/test_cpu.o: $(TEST_DIR)/test_cpu.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/test_rti.o: $(TEST_DIR)/test_rti.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/test_controller.o: $(TEST_DIR)/test_controller.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/test_ppu.o: $(TEST_DIR)/test_ppu.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run tests
tests: $(TEST_TARGET)
	$(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR) *.o
