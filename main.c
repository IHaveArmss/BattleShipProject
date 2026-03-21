#include <stdio.h>

#include "raylib.h"
#include "scripts/widget.h"

//10x10 grid, 4 1x1 3 1x2 2 1x3 1 1x4

typedef enum GameState {
    MENU,
    GAMEPLAY
} GameState;

int main(void) {

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BattleShip");

    InitAudioDevice();
    
    SetTargetFPS(60);

    GameState currentState = MENU;

    while (!WindowShouldClose()) {
        BeginDrawing();

            if(currentState == MENU){
                if(drawMainMenu()){
                    currentState = GAMEPLAY;
                }
            }
            else if(currentState == GAMEPLAY){
                ClearBackground(BACKGROUND_COLOR);
                drawGrid();
            }
                        
            
           
        EndDrawing();
    }
    CloseWindow();
    return 0;
}