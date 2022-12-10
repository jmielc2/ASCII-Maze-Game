#pragma once

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <math.h>
#include <signal.h>

#ifdef DEBUG
#define LOG(...) fprintf(stderr, __VA_ARGS__);
#else
#define LOG(...)
#endif

#define START_DIM_X 11
#define START_DIM_Y 11

#define WALL '#'
#define PATH ' '
#define GOAL 'X'
#define PLAYER 'O'
#define NUM_PUZZLES 5