
#ifndef SCREEN_WIDTH

#include <stdbool.h> 
#include "raylib.h"
#include "gameLogic.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1080
#define BACKGROUND_COLOR CLITERAL(Color){ 0, 0, 0, 0 }//CLITERAL(Color){ 15, 25, 45, 255 }
#define BACKGROUND_COLOR_MENU BLACK



//the 1x1 1x2 1x3 1x4 ships 


void drawGrid(GameState);
bool drawButton(Rectangle,Color,Color);
bool drawMainMenu();
void drawSideMenu(void);
void drawSideTools(void);

#endif