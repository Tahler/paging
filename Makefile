INC_DIR = include
SRC_DIR = src
OUT_DIR = target
OBJ_DIR = $(OUT_DIR)/obj

INC = $(wildcard $(INC_DIR)/*.h)
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC = gcc

CFLAGS_LIB = -g -Wall -fPIC -I$(INC_DIR)
LDFLAGS_LIB = -shared

CFLAGS_EXE = -g -Wall -I$(INC_DIR)
LDFLAGS_EXE =

LIB_NAME = libmmu.so
TARGET_LIB = $(OUT_DIR)/$(LIB_NAME)
EXE_NAME = test
TARGET_EXE = $(OUT_DIR)/$(EXE_NAME)

.PHONY: all
lib: setup $(TARGET_LIB)

exe: setup
	@$(CC) $(CFLAGS_EXE) -c $(SRC) -o $(OBJ)
	@$(CC) $(LDFLAGS_EXE) $(OBJ) -o $(TARGET_EXE)
	@./$(TARGET_EXE)
	@echo All tests passed.

setup:
	@mkdir -p $(OUT_DIR)
	@mkdir -p $(OBJ_DIR)

$(TARGET_LIB): $(OBJ)
	@$(CC) $(LDFLAGS_LIB) $(OBJ) -o $(TARGET_LIB)

$(OBJ): $(SRC) $(INC)
	@$(CC) $(CFLAGS_LIB) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OUT_DIR)
