INC_DIR = include
SRC_DIR = src
OUT_DIR = target
OBJ_DIR = $(OUT_DIR)/obj

INC = $(wildcard $(INC_DIR)/*.h)
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC = gcc
CFLAGS = -g -Wall -fPIC -I$(INC_DIR)
LDFLAGS = -shared

NAME = libmmu.so
TARGET = $(OUT_DIR)/$(NAME)

.PHONY: all
all: setup $(TARGET)

setup:
	@mkdir -p $(OUT_DIR)
	@mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(TARGET)

$(OBJ): $(SRC) $(INC)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OUT_DIR)
