
#ifndef SCREEN_WIDTH

#include <stdbool.h> 
#include "raylib.h"
#include "gameLogic.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1080
#define BACKGROUND_COLOR CLITERAL(Color){ 0, 0, 0, 0 }
#define BACKGROUND_COLOR_MENU BLACK



//the 1x1 1x2 1x3 1x4 ships 

//meniuri
bool drawMainMenu();
int drawPlayerSelect();
void drawConnecting();
void drawWaitingReady();
void drawGameOver();

//grid si componente
void drawGrid(GameState);
bool drawButton(Rectangle,Color,Color);
void drawSideMenu(void);
void drawSideTools(void);
bool drawReadyButton(void);

#endif