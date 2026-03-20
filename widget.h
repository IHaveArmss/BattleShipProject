
#ifndef SCREEN_WIDTH

#include <stdbool.h> 
#include "raylib.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1200
//the 1x1 1x2 1x3 1x4 ships 
#define ONE_SHIP 4
#define TWO_SHIP 3
#define THREE_SHIP 2
#define FOUR_SHIP 1
//
#define BACKGROUND_COLOR CLITERAL(Color){ 0, 0, 0, 0 }//CLITERAL(Color){ 15, 25, 45, 255 }
#define BACKGROUND_COLOR_MENU BLACK
void drawGrid();
bool drawButton(Rectangle,Color,Color);
bool drawMainMenu();

#endif