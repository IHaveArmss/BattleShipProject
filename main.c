#include <stdio.h>

#include "raylib.h"
#include "widget.h"

//10x10 grid, 4 1x1 3 1x2 2 1x3 1 1x4

int main(void) {
    
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BattleShip");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

            ClearBackground(BACKGROUND_COLOR);
            drawGrid();
            drawRadar(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 200.0f);
           
        EndDrawing();
    }
    CloseWindow();
    return 0;
}