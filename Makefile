CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
OBJ = src/table.o src/record.o src/rbtree.o src/commands.o src/utils/hashmap.o src/utils/merge_sort.o src/main.o

all: project

project: $(OBJ)
	$(CC) $(CFLAGS) -o project $(OBJ)

src/main.o: src/main.c include/commands.h include/utils/hashmap.h
	$(CC) $(CFLAGS) -c src/main.c -o $@

src/table.o: src/table.c include/table.h include/record.h include/utils/hashmap.h
	$(CC) $(CFLAGS) -c src/table.c -o $@

src/record.o: src/record.c include/record.h include/table.h
	$(CC) $(CFLAGS) -c src/record.c -o $@

src/rbtree.o: src/rbtree.c include/rbtree.h include/record.h
	$(CC) $(CFLAGS) -c src/rbtree.c -o $@

src/commands.o: src/commands.c include/commands.h include/utils/merge_sort.h include/rbtree.h
	$(CC) $(CFLAGS) -c src/commands.c -o $@

src/utils/hashmap.o: src/utils/hashmap.c include/utils/hashmap.h
	$(CC) $(CFLAGS) -c src/utils/hashmap.c -o $@

src/utils/merge_sort.o: src/utils/merge_sort.c include/utils/merge_sort.h
	$(CC) $(CFLAGS) -c src/utils/merge_sort.c -o $@

clean:
	rm -f src/*.o src/utils/*.o project
