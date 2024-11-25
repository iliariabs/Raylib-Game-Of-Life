#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define GRID_SIZE 500    
#define CELL_SIZE 3      
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600


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
void AddBrushWithMouse(GameOfLife* game, Vector2 mousePosition, int brushSize);

int main(void) {
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game of Life");
    SetTargetFPS(60); 

    
    GameOfLife game;
    InitGameOfLife(&game, GRID_SIZE, GRID_SIZE);

    int iteration = 0;
    int population = 0;
    int speed = 60;  
    bool isPaused = false;  

    char speedText[10] = "10"; 
    bool isSettingSpeed = false;  

    
    float timeAccumulator = 0.0f;
    float updateInterval = 1.0f / speed;  

    int brushSize = 0;
    
    srand(time(NULL));
    for (int y = 0; y < game.height; y++) {
        for (int x = 0; x < game.width; x++) {
            if (rand() % 2) {
                SetCell(&game, x, y, true);
                population++;
            }
        }
    }

    
    while (!WindowShouldClose()) {
        
        float deltaTime = GetFrameTime();
        timeAccumulator += deltaTime;

        
        if (IsKeyDown(KEY_UP)) {
            speed = (speed < 60) ? speed + 1 : 60;
            updateInterval = 1.0f / speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            speed = (speed > 1) ? speed - 1 : 1;  
            updateInterval = 1.0f / speed;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            isPaused = !isPaused;  
        }
        if (IsKeyPressed(KEY_R)) {
            speed = 1;  
            updateInterval = 1.0f / speed;
        }
        if (IsKeyPressed(KEY_M)) {
            speed = 60;  
            updateInterval = 1.0f / speed;
        }
        if (GetMouseWheelMove() > 0) {
            brushSize = (brushSize < 50) ? brushSize + 1 : 50;  
        }
        if (GetMouseWheelMove() < 0) {
            brushSize = (brushSize > 0) ? brushSize - 1 : 0;  
        }


        if (!isPaused) {
            if (timeAccumulator >= updateInterval) {
                UpdateGameOfLife(&game);
                timeAccumulator = 0.0f;
                iteration++;  
            }
        }


        population = 0;

        
        for (int y = 0; y < game.height; y++) {
            for (int x = 0; x < game.width; x++) {
                if (game.grid[y * game.width + x]) {
                    population++;
                }
            }
        }

        
        Vector2 mousePosition = GetMousePosition();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {  
            AddBrushWithMouse(&game, mousePosition, brushSize);
        }

        
        BeginDrawing();
        ClearBackground(BLACK);  

        DrawGameOfLife(&game);  
        DrawStats(iteration, population);  

        if (isPaused) {
            DrawText("PAUSED", 10, 10, 30, YELLOW);  
        }

        EndDrawing();


    }

    
    free(game.grid);
    CloseWindow(); 

    return 0;
}

void InitGameOfLife(GameOfLife* game, int width, int height) {
    game->width = width;
    game->height = height;
    game->grid = (bool*)calloc(width * height, sizeof(bool));  
}

void UpdateGameOfLife(GameOfLife* game) {
    bool* newGrid = (bool*)calloc(game->width * game->height, sizeof(bool));

    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            int neighbors = CountNeighbors(game, x, y);
            bool currentState = game->grid[y * game->width + x];

            
            if (currentState && (neighbors == 2 || neighbors == 3)) {
                newGrid[y * game->width + x] = true; 
            } else if (!currentState && neighbors == 3) {
                newGrid[y * game->width + x] = true; 
            }
        }
    }

    
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
    DrawText(stats, 10, SCREEN_HEIGHT-30, 20, YELLOW);
}

void AddBrushWithMouse(GameOfLife* game, Vector2 mousePosition, int brushSize) {
    int xCenter = (int)(mousePosition.x / CELL_SIZE);
    int yCenter = (int)(mousePosition.y / CELL_SIZE);

    for (int y = -brushSize; y <= brushSize; y++) {
        for (int x = -brushSize; x <= brushSize; x++) {
            if (brushSize == 0) {  
                SetCell(game, xCenter, yCenter, true);  
            } else {  
                for (int y = -brushSize; y <= brushSize; y++) {  
                    for (int x = -brushSize; x <= brushSize; x++) {  
                        SetCell(game, xCenter + x, yCenter + y, true);  
                    }  
                }  
            }
        }
    }
}