CC := clang
CFLAGS := -g -Wall -fsanitize=address

all: pathfinding

clean:
	rm -rf pathfinding pathfinding.dSYM

pathfinding: main.c map.c map.h movelist.c movelist.h util.c util.h
	$(CC) $(CFLAGS) -o pathfinding main.c map.c movelist.c util.c -lncurses

.PHONY: all clean
