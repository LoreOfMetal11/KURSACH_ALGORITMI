#ifndef MAZE_H
#define MAZE_H

#include "difficulty.h"

#define WIDTH  (G_WIDTH)
#define HEIGHT (G_HEIGHT)

extern int** maze;

int  allocMaze(void);
void freeMaze(void);

void initMaze(int startX, int startY, int exitX, int exitY);

#endif
