CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = src/command.c src/file_io.c src/hashmap.c src/rbtree.c src/record.c src/table.c src/utils.c 
OBJ = $(SRC:.c=.o)
TARGET = database

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lpthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)