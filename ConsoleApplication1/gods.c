#include "gods.h"
#include "maze.h"
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct { int x, y; } Point;

const char* ozzy_lines[] = {
    "Sometimes I feel like I'm so empty",
    "Sometimes I feel like I'm alive",
    "So many voices talking to me",
    "Please God just let my mind survive",
    "I see the moon and tears are falling from my eyes",
    "Will I make it through the night?",
    "I guess tonight will be another nightmare just for me",
    "I found myself another God-shaped hole",
    "Will someone pray for me, pray to save my soul for me",
    "The only gods, the gods of rock'n'roll",
    "Believe me, don't try to understand",
    "Just watch the shadows creeping off the walls",
    "Stay with me, don't want to be alone",
    "I need you as another night time falls",
    "With all the songs and dances",
    "Still looking for the answers",
    "I guess I've lost my self control",
    "With all these crazy voices",
    "I'm running out of choices",
    "I need you now, the gods of rock'n'roll"
};
const int ozzy_count = sizeof(ozzy_lines) / sizeof(ozzy_lines[0]);

static void printOzzyAscii(void) {
    printf(
        "             _\n"
        "            /;)\n"
        "           /;(\n"
        "           >_/\n"
        "           |-|\n"
        "           |-|\n"
        "           |-|\n"
        "           |-|\n"
        "           |-|\n"
        "           |-|\n"
        "           |-|\n"
        "       _   |-|\n"
        "      / \\  |-|   _\n"
        "     :   `'|-|  /,\\\n"
        "     :   ,`'-'`'/|:\n"
        "      \\  \\ ...   ;/\n"
        "       :  )...  ::\n"
        "       ; / ...  ::\n"
        "      / /  ___   \\\\\n"
        "     :  `-|||||.  \\:\n"
        "     :        (\\`-';\n"
        "      `._________,' SSt\n"
        "    OZZY FOREVER!\n"
    );
}

static bool isValidLocal(int x, int y, bool** visited) {
    return x > 0 && x < WIDTH - 1 &&
        y > 0 && y < HEIGHT - 1 &&
        maze[y][x] == 0 && !visited[y][x];
}

static bool dfsLocal(int x, int y, int tx, int ty,
    Point* path, int* pathLength, bool** visited) {
    if (x == tx && y == ty) {
        Point temp = { x, y };
        path[(*pathLength)++] = temp;
        return true;
    }
    if (!isValidLocal(x, y, visited)) return false;

    visited[y][x] = true;
    Point temp = { x, y };
    path[(*pathLength)++] = temp;

    int mx[] = { 1,-1,0,0 }, my[] = { 0,0,1,-1 };
    for (int i = 0; i < 4; i++) {
        int nx = x + mx[i], ny = y + my[i];
        if (dfsLocal(nx, ny, tx, ty, path, pathLength, visited))
            return true;
    }
    (*pathLength)--;
    return false;
}

void gods_passage(int startX, int startY, int exitX, int exitY) {
    Point* path = (Point*)malloc(WIDTH * HEIGHT * sizeof(Point));
    if (!path) return;

    bool** visited = (bool**)malloc(HEIGHT * sizeof(bool*));
    if (!visited) { free(path); return; }
    for (int i = 0; i < HEIGHT; i++) {
        visited[i] = (bool*)malloc(WIDTH * sizeof(bool));
        if (!visited[i]) return;
        for (int j = 0; j < WIDTH; j++)
            visited[i][j] = false;
    }

    int pathLength = 0;
    dfsLocal(startX, startY, exitX, exitY, path, &pathLength, visited);

    for (int step = 0; step < pathLength; step++) {
        system("cls");
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (y == path[step].y && x == path[step].x)
                    printf("@");
                else if (y == exitY && x == exitX)
                    printf("E");
                else if (maze[y][x] == 1)
                    printf("#");
                else
                    printf(" ");
            }
            if (y == step && step < ozzy_count)
                printf("   %s", ozzy_lines[step]);
            printf("\n");
        }
        Sleep(900);
    }

    system("cls");
    printOzzyAscii();
    Sleep(3500);

    for (int i = 0; i < HEIGHT; i++)
        free(visited[i]);
    free(visited);
    free(path);
}
