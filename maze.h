#pragma once

typedef struct {
    int xOffset, yOffset;
} direction;

typedef struct {
    int x, y, dir, check;
} entry;

extern entry initEntry(int x, int y, int dir, int check);

enum {UP=0,RIGHT, DOWN, LEFT};
static direction dirs[] = {{0,-1},{1,0},{0,1},{-1,0}};

extern void showGrid();

extern int isValidPos(const int x, const int y);

extern void setGridChar(char grid[], const entry cur, const char a);

extern char getGridChar(char grid[], const entry cur);

extern entry genMaze(int x, int y);