INC_DIR = include
SRC_DIR = src
OUT_DIR = target
OBJ_DIR = $(OUT_DIR)/obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC = gcc
CFLAGS = -g -Wall -I$(INC_DIR)
LDFLAGS = #-shared

NAME = test
TARGET = $(OUT_DIR)/$(NAME)

.PHONY: all
all: setup $(TARGET)

setup:
	@mkdir -p $(OUT_DIR)
	@mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(TARGET)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OUT_DIR)
