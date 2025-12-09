#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include "difficulty.h"

int G_WIDTH = 31;
int G_HEIGHT = 17;
int BOT_DELAY_MS = 900;

void setEasyDifficulty(void) {
    G_WIDTH = 31;
    G_HEIGHT = 17;
    BOT_DELAY_MS = 900;
}

static void printLuckyAscii(void) {
    printf(
        "  ,--.     ,--.\n"
        " (  O )   (  O )\n"
        "  `--'  \\  `--'\n"
        "         \\   _\n"
        "   >-.   /   /|\n"
        "      `-.__.'\n"
    );
    printf("\nнет\n");
    Sleep(2000);
}

void selectDifficulty(void) {
    int choice;
    printf("Выберите сложность:\n");
    printf("1 - Легко  (маленький лабиринт, медленный бот)\n");
    printf("2 - Норма (средний лабиринт, средний бот)\n");
    printf("3 - Жесть (большой лабиринт, быстрый бот)\n");
    printf("4 - мне повезёт\n");
    printf("Ваш выбор: ");
    if (scanf("%d", &choice) != 1) {
        choice = 1;
    }
    while (getchar() != '\n');

    switch (choice) {
    case 1:
        G_WIDTH = 31; G_HEIGHT = 17; BOT_DELAY_MS = 900;
        break;
    case 2:
        G_WIDTH = 41; G_HEIGHT = 21; BOT_DELAY_MS = 600;
        break;
    case 3:
        G_WIDTH = 61; G_HEIGHT = 31; BOT_DELAY_MS = 250;
        break;
    case 4:
        printLuckyAscii();
        G_WIDTH = 61; G_HEIGHT = 31; BOT_DELAY_MS = 250;
        break;
    default:
        G_WIDTH = 41; G_HEIGHT = 21; BOT_DELAY_MS = 600;
        break;
    }

    printf("Сложность: %d x %d, задержка бота %d мс\n",
        G_WIDTH, G_HEIGHT, BOT_DELAY_MS);
    Sleep(1200);
}
