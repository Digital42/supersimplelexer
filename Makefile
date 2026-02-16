# ==========================================================
# Compiler and Flags
# ==========================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -g -Iinclude -I/usr/local/include
# if unity.h is under /usr/local/include/unity
CFLAGS += -I/usr/local/include/unity
LDFLAGS = -L/usr/local/lib -lunity
TARGET  = bin/lexer.bin
TEST_TARGET = bin/tests.bin

# ==========================================================
# Source and Object Files
# ==========================================================

SRC_EXAMPLES = examples/main.c
SRC_LIB      = src/lexer.c
SRC          = $(SRC_EXAMPLES) $(SRC_LIB)

OBJ_DIR      = bin/obj
OBJ 		 = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

# ==========================================================
# Test Files
# ==========================================================

TEST_SRC = tests/lexTest.c
TEST_OBJ = $(OBJ_DIR)/tests/lexTest.o
TEST_LIB_OBJ = $(OBJ_DIR)/src/lexer.o

# Compile object files explicitly
$(OBJ_DIR)/src/lexer.o: src/lexer.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/tests/lexTest.o: tests/lexTest.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build test binary
$(TEST_TARGET): $(TEST_LIB_OBJ) $(TEST_OBJ)
	$(CC) $^ -o $(TEST_TARGET) $(LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)



# Create object directory if it doesn't exist
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p bin)

# ==========================================================
# Default Target
# ==========================================================

all: $(TARGET)

# ==========================================================
# Link Step
# ==========================================================

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# ==========================================================
# Compile Step
# ==========================================================

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ==========================================================
# Utility Targets
# ==========================================================

clean:
	rm -rf bin

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
