#include "stdafx.h"
#include "maze.h"

char *grid = NULL;
extern char* buf;
HANDLE outHandle;
int solved = 1, run = NUM_PUZZLES + 1;

void sigint_handler(int sig) {
    run = 0;
}

void init() {
    outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    printf("\x1b[2J");
    srand((int) time(NULL));
    signal(SIGINT, sigint_handler);
}

void update(int dir, entry *pos) {
    if (dir >= 0) {
        setGridChar(grid, *pos, PATH);
        pos->x += dirs[dir].xOffset;
        pos->y += dirs[dir].yOffset;
        if (isValidPos(pos->x, pos->y) && getGridChar(grid, *pos) != WALL) {
            if (getGridChar(grid, *pos) == GOAL) {
                solved = 1;
            }
            setGridChar(grid, *pos, PLAYER);
        } else {
            pos->x -= dirs[dir].xOffset;
            pos->y -= dirs[dir].yOffset;
            setGridChar(grid, *pos, PLAYER);
        }
    }
}

int main() {
    init();
    entry pos = initEntry(0,0,0,0);
    int a = -1, dir;
    while (run) {
        if (solved) {
            run--;
            pos = genMaze(START_DIM_X + ((NUM_PUZZLES - run) * 4), START_DIM_Y + ((NUM_PUZZLES - run) * 4));
            solved = 0;
        }
        if (run) {
            showGrid();
            while (run) {
                if (kbhit()) {
                    break;
                }
            }
            if (run) {
                a = getch();
            }
            switch (a) {
            case (72):
                dir = UP;
                break;
            case (77):
                dir = RIGHT;
                break;
            case (80):
                dir = DOWN;
                break;
            case (75):
                dir = LEFT;
                break;
            default:
                dir = -1;
                break;
            }
            update(dir, &pos);
        }
    }
    showGrid();
    printf("\nThanks for playing!\n");
    free(grid);
    free(buf);
    return 0;
}