#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define GRID_SIZE 500    // Размер сетки (вы можете увеличить для более крупных клеток)
#define CELL_SIZE 3      // Размер каждой клетки
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Определим структуру для игры
typedef struct {
    bool* grid;
    int width;
    int height;
} GameOfLife;

void InitGameOfLife(GameOfLife* game, int width, int height);
void UpdateGameOfLife(GameOfLife* game);
void DrawGameOfLife(GameOfLife* game);
int CountNeighbors(GameOfLife* game, int x, int y);
void SetCell(GameOfLife* game, int x, int y, bool state);
void DrawStats(int iteration, int population);
void AddCellWithMouse(GameOfLife* game, Vector2 mousePosition);

int main(void) {
    // Инициализация
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game of Life");
    SetTargetFPS(60); // Ограничение кадров в секунду

    // Инициализация состояния игры
    GameOfLife game;
    InitGameOfLife(&game, GRID_SIZE, GRID_SIZE);

    int iteration = 0;
    int population = 0;
    int speed = 10;  // Начальная скорость обновлений (итераций в секунду)
    bool isPaused = false;  // Статус паузы

    char speedText[10] = "10"; // Начальное значение для скорости
    bool isSettingSpeed = false;  // Флаг для ввода новой скорости

    // Время для вычисления интервала обновлений
    float timeAccumulator = 0.0f;
    float updateInterval = 1.0f / speed;  // Интервал обновления в секундах

    // Рандомная начальная популяция
    srand(time(NULL));
    for (int y = 0; y < game.height; y++) {
        for (int x = 0; x < game.width; x++) {
            if (rand() % 2) {
                SetCell(&game, x, y, true);
                population++;
            }
        }
    }

    // Главный игровой цикл
    while (!WindowShouldClose()) {
        // Получаем время прошедшее с последнего кадра
        float deltaTime = GetFrameTime();
        timeAccumulator += deltaTime;

        // Обработка ввода для скорости и паузы
        if (IsKeyDown(KEY_UP)) {
            speed = (speed < 60) ? speed + 1 : 60;
            updateInterval = 1.0f / speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            speed = (speed > 1) ? speed - 1 : 1;  // Уменьшить скорость, не ниже 1
            updateInterval = 1.0f / speed;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            isPaused = !isPaused;  // Переключение состояния паузы
        }
        if (IsKeyPressed(KEY_R)) {
            speed = 1;  // Увеличить скорость
            updateInterval = 1.0f / speed;
        }
        if (IsKeyPressed(KEY_M)) {
            speed = 60;  // Увеличить скорость
            updateInterval = 1.0f / speed;
        }


        if (!isPaused) {
            if (timeAccumulator >= updateInterval) {
                UpdateGameOfLife(&game);
                timeAccumulator = 0.0f;
                iteration++;  // Increment the iteration count
            }
        }


        population = 0;

        // Подсчет живых клеток
        for (int y = 0; y < game.height; y++) {
            for (int x = 0; x < game.width; x++) {
                if (game.grid[y * game.width + x]) {
                    population++;
                }
            }
        }

        // Обработка клика мышью для добавления клеток
        Vector2 mousePosition = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            AddCellWithMouse(&game, mousePosition);
        }

        // Отрисовка
        BeginDrawing();
        ClearBackground(BLACK);  // Ensure the screen is cleared at the beginning of each frame.

        DrawGameOfLife(&game);  // Draw the game grid
        DrawStats(iteration, population);  // Draw the statistics

        if (isPaused) {
            DrawText("PAUSED", 0, 0, 30, RED);  // Display PAUSED text
        }

        EndDrawing();


    }

    // Деинициализация
    free(game.grid);
    CloseWindow(); // Закрыть окно и контекст OpenGL

    return 0;
}

void InitGameOfLife(GameOfLife* game, int width, int height) {
    game->width = width;
    game->height = height;
    game->grid = (bool*)calloc(width * height, sizeof(bool));  // Динамическое выделение памяти
}

void UpdateGameOfLife(GameOfLife* game) {
    bool* newGrid = (bool*)calloc(game->width * game->height, sizeof(bool));

    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            int neighbors = CountNeighbors(game, x, y);
            bool currentState = game->grid[y * game->width + x];

            // Правила игры
            if (currentState && (neighbors == 2 || neighbors == 3)) {
                newGrid[y * game->width + x] = true; // Клетка остается живой
            } else if (!currentState && neighbors == 3) {
                newGrid[y * game->width + x] = true; // Клетка становится живой
            }
        }
    }

    // Обновление состояния сетки
    free(game->grid);
    game->grid = newGrid;
}

void DrawGameOfLife(GameOfLife* game) {
    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            if (game->grid[y * game->width + x]) {
                Vector2 position = {(float)x * CELL_SIZE, (float)y * CELL_SIZE};
                DrawRectangleV(position, (Vector2){CELL_SIZE, CELL_SIZE}, RAYWHITE);
            }
        }
    }
}

int CountNeighbors(GameOfLife* game, int x, int y) {
    int neighbors = 0;
    for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
            if (i == 0 && j == 0) continue;

            int nx = (x + i + game->width) % game->width;
            int ny = (y + j + game->height) % game->height;

            if (game->grid[ny * game->width + nx]) {
                neighbors++;
            }
        }
    }
    return neighbors;
}

void SetCell(GameOfLife* game, int x, int y, bool state) {
    if (x >= 0 && x < game->width && y >= 0 && y < game->height) {
        game->grid[y * game->width + x] = state;
    }
}

void DrawStats(int iteration, int population) {
    char stats[128];
    snprintf(stats, sizeof(stats), "Iteration: %d | Population: %d", iteration, population);
    DrawText(stats, 10, SCREEN_HEIGHT / 2 - 50, 20, RAYWHITE);
}

void AddCellWithMouse(GameOfLife* game, Vector2 mousePosition) {
    // Конвертируем координаты мыши в координаты клетки
    int x = (int)(mousePosition.x / CELL_SIZE);
    int y = (int)(mousePosition.y / CELL_SIZE);

    // Добавляем клетку
    SetCell(game, x, y, true);
}
