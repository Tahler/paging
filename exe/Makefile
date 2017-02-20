INC_DIR = include
SRC_DIR = src
LIB_DIR = lib
OUT_DIR = target
OBJ_DIR = $(OUT_DIR)/obj

INC = $(wildcard $(INC_DIR)/*.h)
SRC = $(wildcard $(SRC_DIR)/*.c)
# LIB = $(wildcard $(LIB_DIR)/*.so)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC = gcc

CFLAGS = -g -Wall -I$(INC_DIR)
LDFLAGS = -L$(LIB_DIR) -lmmu -Wl,-rpath,$(LIB_DIR)

NAME = grader
TARGET = $(OUT_DIR)/$(NAME)

.PHONY: exe
exe: setup
	@$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)
	@$(CC) $(LDFLAGS) $(OBJ) -o $(TARGET)

.PHONY: test
test: exe
	@./$(TARGET)
	@echo All tests passed.

.PHONY: setup
setup:
	@mkdir -p $(OUT_DIR)
	@mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJ)
	@$(CC) $(LDFLAGS_LIB) $(OBJ) -o $(TARGET)

$(OBJ): $(SRC) $(INC)
	@$(CC) $(CFLAGS_LIB) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OUT_DIR)
