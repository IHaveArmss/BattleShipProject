#include <stdio.h>

#include "raylib.h"
#include "widget.h"

//10x10 grid, 4 1x1 3 1x2 2 1x3 1 1x4

int main(void) {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BattleShip");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            for(int i=1;i<=10;i)
            DrawRectangle(1,1,50,50,RED);
            DrawText("CONGRATS",190,200,20,LIGHTGRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}