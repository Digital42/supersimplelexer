# ==========================================================
# Compiler and Flags
# ==========================================================
CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -g -Iinclude -I/usr/local/include
CFLAGS += -I/usr/local/include/unity      # if unity.h is under /usr/local/include/unity
LDFLAGS = -L/usr/local/lib -lunity

TARGET       = bin/lexer.bin
TEST_TARGET  = bin/tests.bin

# ==========================================================
# Source Files
# ==========================================================
SRC_EXAMPLES = examples/main.c
SRC_LEX      = src/lexer.c
SRC_HASH      = src/hash.c
SRC          = $(SRC_EXAMPLES) $(SRC_LEX) $(SRC_HASH)

TEST_SRC     = tests/lexTest.c
TEST_LEX_SRC = src/lexer.c
TEST_HASH_SRC = src/hash.c
TEST          = $(TEST_SRC) $(TEST_LEX_SRC) $(TEST_HASH_SRC)
# ==========================================================
# Object Files (compiled into bin/obj)
# ==========================================================
OBJ_DIR      = bin/obj
OBJ          = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
TEST_OBJ     = $(patsubst %.c,$(OBJ_DIR)/%.o,$(TEST_SRC))
TEST_LIB_OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(TEST))

# ==========================================================
# Pattern Rule: Compile any .c into bin/obj/*.o
# ==========================================================
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ==========================================================
# Default Target: Build normal binary
# ==========================================================
all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

# ==========================================================
# Test Target
# ==========================================================
$(TEST_TARGET): $(TEST_LIB_OBJ) $(TEST_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

# ==========================================================
# Utility Targets
# ==========================================================
clean:
	rm -rf bin

.PHONY: all run test clean