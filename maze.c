#include "stdafx.h"
#include "maze.h"

entry initEntry(int x, int y, int dir, int check) {
    entry newEntry;
    newEntry.x = x;
    newEntry.y = y;
    newEntry.dir = dir;
    newEntry.check = check;
    return newEntry;
}

int DIM_X, DIM_Y;
extern char *grid;
char *buf = NULL;
static char *explored;
static entry *container;
extern HANDLE outHandle;
extern int run;

void showGrid() {
    const int size = sizeof(char) * ((2 * DIM_X) + 4) * (DIM_Y + 2);
    memset(buf, '#', size);
    for (int i = 0; i < DIM_Y + 2; i++) {
        if (i == 0 || i == DIM_Y + 1) {
            for (int j = 1; j < ((2 * DIM_X) + 4); j += 2) {
                buf[i * ((2 * DIM_X) + 4) + j] = ' ';
            }
        } else {
            for (int j = 0; j < DIM_X; j++) {
                buf[i * ((2 * DIM_X) + 4) + 2 + (j * 2)] = grid[(DIM_X * (i - 1)) + j];
                buf[i * ((2 * DIM_X) + 4) + 1 + (j * 2)] = ' ';
            }
        }
        buf[i * ((2 * DIM_X) + 4) + ((2 * DIM_X) + 1)] = ' ';
        buf[i * ((2 * DIM_X) + 4) + ((2 * DIM_X) + 3)] = '\n';
    }
#ifndef DEBUG
    printf("\x1b[H\nMaze %d of %d:\n", NUM_PUZZLES - run + 1, NUM_PUZZLES);
#endif
    WriteConsole(outHandle, buf, size, 0, 0);
    fflush(stdout);
}

int isValidPos(const int x, const int y) {
    return !(x < 0 || y < 0 || x >= DIM_X || y >= DIM_Y);
}

void setGridChar(char grid[], const entry cur, const char a) {
#ifdef DEBUG
    if (!isValidPos(cur.x, cur.y)) {
        LOG("ERROR: setGridChar function failed -> out of bounds\n");
        return;
    }
#endif
    grid[(DIM_X * cur.y) + cur.x] = a;
}

char getGridChar(char grid[], const entry cur) {
#ifdef DEBUG
    if (!isValidPos(cur.x, cur.y)) {
        LOG("ERROR: getGridChar function failed -> out of bounds\n");
        return -1;
    }
#endif
    return grid[(DIM_X * cur.y) + cur.x];
}

static int push_back(int *top, entry newEntry) {
#ifdef DEBUG
    if (*top >= DIM_X * DIM_Y) {
        LOG("ERROR: push_back function failed -> stack is full");
        return 0;
    }
#endif
    (*top)++;
    container[*top] = newEntry;
    return 1;
}

static int pop_back(int *top) {
#ifdef DEBUG
    if (*top < 0) {
        LOG("ERROR: pop function failed -> stack is empty");
        return 0;
    }
#endif
    (*top)--;
    return 1;
}

static int pop_front(int *top) {
#ifdef DEBUG
    if (*top < 0) {
        LOG("ERROR: pop_front function failed -> stack is empty");
        return 0;
    }
#endif
    for (int i = 1; i <= *top; i++) {
        container[i - 1] = container[i];
    }
    (*top)--;
    return 1;
}

static int isWall(entry cur, int dir) {
    cur.x += dirs[dir].xOffset;
    cur.y += dirs[dir].yOffset;
    return (!isValidPos(cur.x, cur.y) || getGridChar(grid, cur) == WALL);
}

static int canClearWall(entry cur) {
    int count = 0;
    for (int i = 0; i < 4; i++) {
        count += isWall(cur, i);
    }
    return count;
}

static void addNeighbors(int* top, entry node) {
    entry temp;
    for (int i = 0; i < 4; i++) {
        temp = node;
        temp.x += dirs[i].xOffset;
        temp.y += dirs[i].yOffset;
        if (isValidPos(temp.x, temp.y) && !getGridChar(explored, temp)) {
            if (getGridChar(grid, temp) == WALL) {
                setGridChar(explored, temp, (char) 1);
            } else {
                push_back(top, temp);
            }
        }
    }
}

static entry farthestNode() {
    memset(explored, 0, DIM_X * DIM_Y);
    int top = -1;

    int x = 0, y = 0;
    push_back(&top, initEntry(x, y, 0, 0));
    entry node;
    while (top >= 0) {
        int count = top + 1;
        while (count > 0) {
            node = container[0];
            pop_front(&top);
            if (!getGridChar(explored, node)) {
                setGridChar(explored, node, (char) 1);
                addNeighbors(&top, node);
            }
            count--;
        }
    }
    return node;
}

entry genMaze(int x, int y) {
    DIM_X = x;
    DIM_Y = y;

    if (grid) {
        free(grid);
    }
    grid = (char*) malloc(sizeof(char) * DIM_X * DIM_Y);
    if (buf) {
        free(buf);
    }
    buf = (char*) malloc(sizeof(char) * ((2 * DIM_X) + 4) * (DIM_Y + 2));
    container = (entry*) malloc(sizeof(entry) * DIM_X * DIM_Y);
    explored = (char*) malloc(sizeof(char) * DIM_X * DIM_Y);
    memset(explored, 0, DIM_X * DIM_Y);
    memset(grid, WALL, DIM_X * DIM_Y);
    int top = -1;

    x = 0; y = 0;
    int dir = rand() % 4;
    entry start = initEntry(x, y, dir, 1);
    push_back(&top, start);
    while (top >= 0) {
        entry node = container[top];
        pop_back(&top);
        
        if ((int) getGridChar(explored, node) || canClearWall(node) < 3) {
            continue;
        }
        setGridChar(grid, node, PATH);
        setGridChar(explored, node, (char) 1);

        if (node.check) {
            if (rand() % 100 >= 30) {
                node.dir = rand() % 4;
            }
            for (int i = 0; i < 4; i++) {
                if (i == node.dir) {
                    continue;
                }
                x = node.x + dirs[i].xOffset;
                y = node.y + dirs[i].yOffset;
                if (isValidPos(x, y)) {
                    push_back(&top, initEntry(x, y, i, !node.check));
                }
            }
        }
        x = node.x + dirs[node.dir].xOffset;
        y = node.y + dirs[node.dir].yOffset;
        if (isValidPos(x, y)) {
            push_back(&top, initEntry(x, y, node.dir, !node.check));
        }
    }
    setGridChar(grid, start, 'O');
    setGridChar(grid, farthestNode(), 'X');
    free(container);
    free(explored);
    return start;
}