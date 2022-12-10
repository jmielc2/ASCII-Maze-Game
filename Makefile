DEBUG_FLAGS=-g -Wall -Werror -DDEBUG
RELEASE_FLAGS=-Wall -O3

all: release

debug: main.c
	gcc ${DEBUG_FLAGS} main.c maze.c -o maze-d

release: main.c
	gcc ${RELEASE_FLAGS} main.c maze.c -o maze