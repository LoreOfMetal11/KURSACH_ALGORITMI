#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#include <windows.h>
#include <time.h>

#include "difficulty.h"
#include "maze.h"
#include "gods.h"

#define MAXNAME 67

typedef struct { int x, y; } Point;

int playerScore = 0;
int botScore = 0;

Point path[40000];
int pathLength = 0;

bool** visited = NULL;

Point fullPath[40000];
int fullPathLen = 0;

void saveScore(void);

int allocVisited(void) {
    visited = (bool**)malloc(HEIGHT * sizeof(bool*));
    if (!visited) return 0;
    for (int i = 0; i < HEIGHT; i++) {
        visited[i] = (bool*)malloc(WIDTH * sizeof(bool));
        if (!visited[i]) return 0;
    }
    return 1;
}

void freeVisited(void) {
    if (!visited) return;
    for (int i = 0; i < HEIGHT; i++)
        free(visited[i]);
    free(visited);
    visited = NULL;
}

bool manualIsValid(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && maze[y][x] == 0;
}

bool isValid(int x, int y) {
    return x > 0 && x < WIDTH - 1 && y > 0 && y < HEIGHT - 1 &&
        maze[y][x] == 0 && !visited[y][x];
}

void clearVisited() {
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            visited[i][j] = false;
}

bool dfs(int x, int y, int tx, int ty) {
    if (x == tx && y == ty) {
        Point temp; temp.x = x; temp.y = y;
        path[pathLength++] = temp;
        return true;
    }
    if (!isValid(x, y)) return false;
    visited[y][x] = true;
    Point temp; temp.x = x; temp.y = y;
    path[pathLength++] = temp;
    int mx[] = { 1,-1,0,0 }, my[] = { 0,0,1,-1 };
    for (int i = 0; i < 4; i++) {
        int nx = x + mx[i], ny = y + my[i];
        if (dfs(nx, ny, tx, ty)) return true;
    }
    pathLength--;
    return false;
}

void printMazeSingle(int px, int py, int ex, int ey) {
    system("cls");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == py && j == px)
                printf("@");
            else if (i == ey && j == ex)
                printf("E");
            else if (maze[i][j] == 1)
                printf("#");
            else
                printf(" ");
        }
        printf("\n");
    }
}

void printMazeCompetitive(int px, int py, int cx, int cy, int ex, int ey) {
    system("cls");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == py && j == px && i == cy && j == cx)
                printf("*");
            else if (i == py && j == px)
                printf("@");
            else if (i == cy && j == cx)
                printf("&");
            else if (i == ey && j == ex)
                printf("E");
            else if (maze[i][j] == 1)
                printf("#");
            else
                printf(" ");
        }
        printf("\n");
    }
}

void moveThroughPath(int exitX, int exitY) {
    for (int i = 0; i < pathLength; i++) {
        printMazeSingle(path[i].x, path[i].y, exitX, exitY);
        Sleep(120);
    }
}

void makeFullDfsPath(int startX, int startY, int exitX, int exitY) {
    clearVisited();
    pathLength = 0;
    dfs(startX, startY, exitX, exitY);
    fullPathLen = pathLength;
    for (int i = 0; i < fullPathLen; i++) fullPath[i] = path[i];
}

void showRouteHint(int px, int py) {
    int i;
    for (i = 0; i < fullPathLen - 1; i++) {
        if (fullPath[i].x == px && fullPath[i].y == py) {
            int nx = fullPath[i + 1].x, ny = fullPath[i + 1].y;
            printf("GPS: Идите в ");
            if (ny < py) printf("вверх (w)\n");
            else if (ny > py) printf("вниз (s)\n");
            else if (nx < px) printf("влево (a)\n");
            else if (nx > px) printf("вправо (d)\n");
            printf("Осталось шагов до пункта назначения: %d\n", fullPathLen - i - 1);
            return;
        }
    }
    printf("GPS: Игрок, ты ушел с маршрута. Вернись.\n");
}

void saveTimeToFile(const char* name, const char* who, double seconds) {
    FILE* f = fopen("results.txt", "a");
    if (f) {
        fprintf(f, "%s; %s; %.2f сек\n", name, who, seconds);
        fclose(f);
    }
}

void saveScore(void) {
    FILE* f = fopen("results.txt", "a");
    if (f) {
        fprintf(f, "игрок - компьютер %d-%d\n", playerScore, botScore);
        fclose(f);
    }
}

void showResultsFromFile(void) {
    FILE* f = fopen("results.txt", "r");
    char buf[200];
    if (!f) printf("Файл результатов не найден\n");
    else {
        printf("Имя; тип; время / счёт:\n");
        while (fgets(buf, sizeof(buf), f)) printf("%s", buf);
        fclose(f);
        printf("(Нажмите Enter для продолжения)\n");
        getchar();
    }
}

void competitiveMode(void) {
    freeMaze();
    freeVisited();
    if (!allocMaze() || !allocVisited()) {
        printf("Ошибка выделения памяти.\n");
        return;
    }

    int startX = 1, startY = 1;
    int exitX = WIDTH - 2, exitY = HEIGHT - 1;

    int attempts = 0;
    do {
        initMaze(startX, startY, exitX, exitY);
        makeFullDfsPath(startX, startY, exitX, exitY);
        attempts++;
    } while (fullPathLen < 2 && attempts < 20);

    int px = startX, py = startY;
    int cx = startX, cy = startY;
    int botIndex = 0;
    clock_t lastBotMove = clock();
    int playerFinished = 0;
    int botFinished = 0;

    while (!playerFinished && !botFinished) {
        printMazeCompetitive(px, py, cx, cy, exitX, exitY);
        printf("Соревнование! @ - ты, & - компьютер.\n");
        printf("Кто первым дойдёт до E, тот и победил.\n");
        printf("Управление: w/s/a/d + Enter. Бот ходит автоматически.\n");

        char key = getchar();
        int nx = px, ny = py;
        if (key == 'w') ny--;
        else if (key == 's') ny++;
        else if (key == 'a') nx--;
        else if (key == 'd') nx++;
        if (manualIsValid(nx, ny)) { px = nx; py = ny; }

        if (px == exitX && py == exitY) playerFinished = 1;

        clock_t now = clock();
        double ms = (double)(now - lastBotMove) * 1000.0 / CLOCKS_PER_SEC;
        if (ms >= BOT_DELAY_MS && botIndex < fullPathLen) {
            cx = fullPath[botIndex].x;
            cy = fullPath[botIndex].y;
            botIndex++;
            lastBotMove = now;
            if (cx == exitX && cy == exitY) botFinished = 1;
        }
    }

    printMazeCompetitive(px, py, cx, cy, exitX, exitY);

    if (playerFinished && !botFinished) {
        printf("Ты победил. Игрок дошёл до выхода первым.\n");
        playerScore++;
        saveScore();
    }
    else if (!playerFinished && botFinished) {
        printf("Компьютер победил. Комп дошёл до выхода первым.\n");
        botScore++;
        saveScore();
    }
    else {
        printf("Ничья.\n");
        playerScore++;
        botScore++;
        saveScore();
    }

    printf("(Нажмите Enter для продолжения)\n");
    getchar();
    getchar();
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand((unsigned int)time(NULL));

    selectDifficulty();

    if (!allocMaze() || !allocVisited()) {
        printf("Ошибка выделения памяти.\n");
        return 1;
    }

    int startX = 1, startY = 1;
    int exitX = WIDTH - 2, exitY = HEIGHT - 1;

    int attempts = 0;
    do {
        initMaze(startX, startY, exitX, exitY);
        makeFullDfsPath(startX, startY, exitX, exitY);
        attempts++;
    } while (fullPathLen < 2 && attempts < 20);
    if (attempts >= 20)
        printf("Лабиринт не самый удачный, но используется последний.\n");

    int menu_choice, running = 1;

    while (running) {
        printf("Меню:\n");
        printf("1 - Пройти лабиринт самому\n");
        printf("2 - Пройти лабиринт самому, но с подсказками\n");
        printf("3 - Пусть пройдут за меня, мне лень\n");
        printf("4 - Секретный секрет\n");
        printf("5 - Посмотреть результаты\n");
        printf("6 - Соревнование. игрок vs компьютер\n");
        printf("7 - Сменить сложность (и размер лабиринта)\n");
        printf("8 - Выход\n");
        printf("Ваш выбор: ");
        scanf_s("%d", &menu_choice);
        getchar();

        if (menu_choice == 1 || menu_choice == 2) {
            char name[MAXNAME];
            printf("Введите ваше имя: ");
            fgets(name, MAXNAME, stdin);
            name[strcspn(name, "\n")] = 0;
            int px = startX, py = startY, win = 0;
            clock_t start_t = clock();
            while (!win) {
                printMazeSingle(px, py, exitX, exitY);
                if (menu_choice == 2) showRouteHint(px, py);
                printf("Управление: w/s/a/d + Enter\n");
                char key = getchar();
                int nx = px, ny = py;
                if (key == 'w') ny--;
                else if (key == 's') ny++;
                else if (key == 'a') nx--;
                else if (key == 'd') nx++;
                if (manualIsValid(nx, ny)) { px = nx; py = ny; }
                if (px == exitX && py == exitY) {
                    system("cls");
                    printf("Ты выбрался.\n");
                    win = 1;
                    clock_t end_t = clock();
                    double seconds = (double)(end_t - start_t) / CLOCKS_PER_SEC;
                    saveTimeToFile(name, "пользователь", seconds);
                    printf("Время прохождения: %.2f секунд\n", seconds);
                    Sleep(1200);
                }
            }
        }
        else if (menu_choice == 3) {
            clock_t start_t = clock();
            makeFullDfsPath(startX, startY, exitX, exitY);
            pathLength = fullPathLen;
            for (int i = 0; i < pathLength; i++) path[i] = fullPath[i];
            printf("Решение компьютером:\n");
            moveThroughPath(exitX, exitY);
            system("cls");
            printf("Компьютер выбрался.\n");
            clock_t end_t = clock();
            double seconds = (double)(end_t - start_t) / CLOCKS_PER_SEC;
            saveTimeToFile("Компьютер", "ассистент", seconds);
            printf("Время прохождения: %.2f секунд\n", seconds);
            Sleep(1200);
        }
        else if (menu_choice == 4) {
            gods_passage(startX, startY, exitX, exitY);
        }
        else if (menu_choice == 5) {
            showResultsFromFile();
        }
        else if (menu_choice == 6) {
            competitiveMode();
            freeMaze();
            freeVisited();
            if (!allocMaze() || !allocVisited()) {
                printf("Ошибка выделения памяти.\n");
                return 1;
            }
            startX = 1; startY = 1;
            exitX = WIDTH - 2; exitY = HEIGHT - 1;
            attempts = 0;
            do {
                initMaze(startX, startY, exitX, exitY);
                makeFullDfsPath(startX, startY, exitX, exitY);
                attempts++;
            } while (fullPathLen < 2 && attempts < 20);
        }
        else if (menu_choice == 7) {
            freeMaze();
            freeVisited();
            selectDifficulty();
            if (!allocMaze() || !allocVisited()) {
                printf("Ошибка выделения памяти.\n");
                return 1;
            }
            startX = 1; startY = 1;
            exitX = WIDTH - 2; exitY = HEIGHT - 1;
            attempts = 0;
            do {
                initMaze(startX, startY, exitX, exitY);
                makeFullDfsPath(startX, startY, exitX, exitY);
                attempts++;
            } while (fullPathLen < 2 && attempts < 20);
        }
        else if (menu_choice == 8)
            running = 0;
        else
            printf("Некорректный выбор\n");
    }

    freeMaze();
    freeVisited();

    system("cls");
    printf("Выход из программы.\n");
    return 0;
}
