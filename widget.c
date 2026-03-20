#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


#include "widget.h"
#include "raylib.h"
#include "stdbool.h"
#include "animations.h"


#define CHECKER_BOARD 0
#define POS_OFFSET_X 225
#define POS_OFFSET_Y 50
#define HEIGHT_PADDING 600
#define CELLSIZE 55

#define CHECKERBOARD

bool drawButton(Rectangle bounds,Color baseColor,Color gridColor){
    Vector2 mousePoint = GetMousePosition();
    bool clicked = false;
    Color drawColor = baseColor;

    if (CheckCollisionPointRec(mousePoint, bounds)) {
        drawColor = ColorBrightness(baseColor, 0.2f); 
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            clicked = true;
        }
    }

    DrawRectangleRec(bounds, drawColor);
    DrawRectangleLinesEx(bounds, 2, gridColor);

    return clicked;
}

void drawGrid(){
    Vector2 mousePoint = GetMousePosition();

    Color lineCol = GREEN;
    Color fillCol = BLACK;

    for(int k=0;k<=1;k++){
        int yOffset =0;
        if( k ==1){
            yOffset = HEIGHT_PADDING;
        }
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {

                int posX = POS_OFFSET_X + j * CELLSIZE;
                int posY = yOffset + i * CELLSIZE;
                Rectangle bounds = {posX, posY+30, CELLSIZE, CELLSIZE};
                

                if (CheckCollisionPointRec(mousePoint, bounds)) {
                    fillCol = DARKGRAY; 

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        printf("Grid %d-Atacat:[%d][%d]\n", k,i, j);
                    }
                } else {
                    fillCol = BLACK;
                }

                if (CHECKER_BOARD) {
                    Color squareColor;
                    if((i+j)%2==0){
                        squareColor = BLACK;
                    }
                    else
                        squareColor = RED;
                    DrawRectangle(posX, posY, CELLSIZE, CELLSIZE, squareColor);
                } else {
                    
                    drawButton(bounds,fillCol,lineCol);

                }
            }
        }
    }
}


bool drawMainMenu(void){
    ClearBackground(BACKGROUND_COLOR_MENU);

    DrawText("BATTLESHIP", 150, 150, 100, WHITE);
    
    drawRadar(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 200.0f);

    Rectangle btnRect = {350, 900, 300, 100};
    bool isStartClicked = drawButton(btnRect, BLACK, GREEN);

    const char* text = "START";
    int fontSize = 50;
    int textWidth = MeasureText(text, fontSize);
    int textX = btnRect.x + (btnRect.width - textWidth) / 2;
    int textY = btnRect.y + (btnRect.height - fontSize) / 2;
    DrawText(text, textX, textY, fontSize, WHITE);

    return isStartClicked;
}

