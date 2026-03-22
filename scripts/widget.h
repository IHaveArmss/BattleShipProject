
#ifndef SCREEN_WIDTH

#include <stdbool.h> 
#include "raylib.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1200
//the 1x1 1x2 1x3 1x4 ships 

typedef enum GameState {
    MENU,
    GAMEPLAY,
    SETUP,
    ENEMY_TURN,
    PLAYER_TURN
} GameState;

#define BACKGROUND_COLOR CLITERAL(Color){ 0, 0, 0, 0 }//CLITERAL(Color){ 15, 25, 45, 255 }
#define BACKGROUND_COLOR_MENU BLACK
void drawGrid(GameState);
bool drawButton(Rectangle,Color,Color);
bool drawMainMenu();
void drawSideMenu(void);

#endif