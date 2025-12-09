#include "maze.h"
#include <stdlib.h>
#include <time.h>

int** maze = NULL;

static int dx[] = { 2, -2, 0, 0 };
static int dy[] = { 0, 0, 2, -2 };

int allocMaze(void) {
    maze = (int**)malloc(HEIGHT * sizeof(int*));
    if (!maze) return 0;
    for (int i = 0; i < HEIGHT; i++) {
        maze[i] = (int*)malloc(WIDTH * sizeof(int));
        if (!maze[i]) return 0;
    }
    return 1;
}

void freeMaze(void) {
    if (!maze) return;
    for (int i = 0; i < HEIGHT; i++)
        free(maze[i]);
    free(maze);
    maze = NULL;
}

static int isInside(int x, int y) {
    return x > 0 && x < WIDTH - 1 && y > 0 && y < HEIGHT - 1;
}

static void shuffle(int* array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i]; array[i] = array[j]; array[j] = temp;
    }
}

static void generateMaze(int x, int y) {
    int dirs[4] = { 0,1,2,3 };
    shuffle(dirs, 4);
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[dirs[i]], ny = y + dy[dirs[i]];
        if (isInside(nx, ny) && maze[ny][nx] == 1) {
            maze[ny][nx] = 0;
            maze[y + dy[dirs[i]] / 2][x + dx[dirs[i]] / 2] = 0;
            generateMaze(nx, ny);
        }
    }
}

void initMaze(int startX, int startY, int exitX, int exitY) {
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            maze[i][j] = 1;
    maze[startY][startX] = 0;
    generateMaze(startX, startY);
    maze[exitY][exitX] = 0;
}
