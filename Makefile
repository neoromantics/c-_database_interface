CC=g++
CFLAGS=-O3 -Iinclude
EXTRAFLAGS=-lpqxx -lpq
SRC_DIR=src
INCLUDE_DIR=include
OBJ_DIR=obj

SRCS=$(wildcard $(SRC_DIR)/*.cpp)
OBJS=$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: test

test: $(OBJS)
	$(CC) $(CFLAGS) -o test $(OBJS) $(EXTRAFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) test

clobber:
	rm -rf $(OBJ_DIR)
