#include <stdio.h>

#include "raylib.h"
#include "scripts/widget.h"

//10x10 grid, 4 1x1 3 1x2 2 1x3 1 1x4



int main(void) {

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BattleShip");

    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);

    int targetWidth = SCREEN_WIDTH;
    int targetHeight = SCREEN_HEIGHT;

    int horizontalMargin = 80;
    int verticalMargin = 80;

    if (targetWidth > monitorWidth - horizontalMargin) {
        targetWidth = monitorWidth - horizontalMargin;
    }
    if (targetHeight > monitorHeight - verticalMargin) {
        targetHeight = monitorHeight - verticalMargin;
    }

    if (targetWidth < 900) targetWidth = 900;
    if (targetHeight < 700) targetHeight = 700;

    SetWindowSize(targetWidth, targetHeight);

    InitAudioDevice();
    
    SetTargetFPS(60);

    GameState currentState = GAMEPLAY;
    
    while (!WindowShouldClose()) {
        BeginDrawing();

            if(currentState == MENU){
                if(drawMainMenu()){
                    currentState = SETUP;
                }
            }
            else {
                ClearBackground(BACKGROUND_COLOR);
                drawGrid(currentState);

                if(currentState!=ENEMY_TURN && currentState !=PLAYER_TURN)
                    drawSideMenu();
                drawSideTools();
            }
        

        EndDrawing();
    }
    CloseWindow();
    return 0;
}