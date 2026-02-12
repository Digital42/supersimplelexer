# ==========================================================
# Compiler and Flags
# ==========================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -g -Iinclude
LDFLAGS = 
TARGET  = bin/lexer.bin

# ==========================================================
# Source and Object Files
# ==========================================================

SRC_EXAMPLES = examples/main.c
SRC_LIB      = src/lexer.c
SRC          = $(SRC_EXAMPLES) $(SRC_LIB)

OBJ_DIR      = bin/obj
OBJ 		 = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

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
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
