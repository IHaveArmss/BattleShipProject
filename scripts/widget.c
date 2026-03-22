#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


#include "widget.h"
#include "raylib.h"
#include "stdbool.h"
#include "animations.h"
#include "rlgl.h"


#define CHECKER_BOARD 0
#define POS_OFFSET_X 225
#define POS_OFFSET_Y 50
#define HEIGHT_PADDING 600
#define CELLSIZE 55
#define DARKRED CLITERAL(Color){ 220, 20, 60, 255 }
#define CHECKERBOARD


typedef enum tools{
    toolFire,

    toolMarkMaybe,
    toolMarkWater,
    toolClearMark
}Tools;

typedef enum ShipType {
    NULL_SHIP,
    SHIP_1,
    SHIP_2,
    SHIP_3,
    SHIP_4
} ShipType;

typedef struct Ship{
    ShipType type;
    bool status;
}ShipBuild;


int topGridAttacks[10][10] = {0};
ShipBuild PlayerShipMatrix[10][10] ;

int shipsNeeded[5] = {0, 4, 3, 2, 1};
int shipsFound[5]  = {0, 0, 0, 0, 0};
bool boardHasErrors = false;
int tempR[100], tempC[100];

void FloodFillShip(int i, int j, int* minI, int* maxI, int* minJ, int* maxJ, int* count, bool visited[10][10]) {
    if (i < 0 || i >= 10 || j < 0 || j >= 10) return;
    if (PlayerShipMatrix[i][j].type == NULL_SHIP || visited[i][j]) return;

    visited[i][j] = true;
    
    tempR[*count] = i;
    tempC[*count] = j;
    (*count)++;
    if (i < *minI) *minI = i;
    if (i > *maxI) *maxI = i;
    if (j < *minJ) *minJ = j;
    if (j > *maxJ) *maxJ = j;

    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            if (di != 0 || dj != 0) {
                FloodFillShip(i + di, j + dj, minI, maxI, minJ, maxJ, count, visited);
            }
        }
    }
}

void CalculateFleet() {
    bool visited[10][10] = {false};
    boardHasErrors = false; 
    for(int i=0; i<5; i++) shipsFound[i] = 0;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (PlayerShipMatrix[i][j].type != NULL_SHIP && !visited[i][j]) {
                int minI = i, maxI = i, minJ = j, maxJ = j, count = 0;
                
                FloodFillShip(i,j,&minI,&maxI,&minJ,&maxJ,&count,visited);

                int width = maxJ - minJ + 1;
                int height = maxI - minI + 1;

                bool isShipValid = ((width==1||height==1)&&(width*height==count)&&(count <= 4));
                
                if (isShipValid) {
                    shipsFound[count]++;
                } else {
                    boardHasErrors = true;
                }
                for(int k = 0; k < count; k++) {
                    PlayerShipMatrix[tempR[k]][tempC[k]].status = isShipValid;
                }
            }
        }
    }
}



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
void drawRectWBorder(Rectangle bounds,Color baseColor,Color gridColor){
    DrawRectangleRec(bounds, baseColor);
    DrawRectangleLinesEx(bounds, 2, gridColor);
}

void drawX(int posx, int posy, int size) {
    float padding = 5.0f; 
    float thick = 4.0f; //grosime

    Vector2 start1 = { posx + padding, posy +padding };
    Vector2 end1   = { posx+size-padding,posy + size-padding };
    DrawLineEx(start1, end1, thick, RED);

    Vector2 start2 = { posx+size- padding, posy+ padding };
    Vector2 end2   = { posx+padding, posy +size- padding };
    DrawLineEx(start2, end2, thick, RED);
}

void drawTargetMark(int posx, int posy, int size) {
    float thickness = 3.0f;
    int centerX = posx+size/2;
    int centerY = posy+size/2;
    int radius = size/3; 
    int padding = 6;       
    //punct
    DrawCircle(centerX, centerY, 2, DARKRED);
    //outer big ring
    DrawRing((Vector2){ centerX, centerY }, radius - thickness, radius, 0, 360, 36, DARKRED);
    //alea 4 fancy le urasc
    DrawLineEx((Vector2){ centerX, posy+ padding }, (Vector2){ centerX, centerY -radius }, thickness, DARKRED);
    DrawLineEx((Vector2){ centerX,centerY +radius }, (Vector2){ centerX, posy + size -padding }, thickness, DARKRED);
    DrawLineEx((Vector2){ posx +padding, centerY }, (Vector2){ centerX -radius, centerY }, thickness, DARKRED);
    DrawLineEx((Vector2){ centerX + radius, centerY }, (Vector2){ posx+ size - padding, centerY }, thickness, DARKRED);
}

void drawShipPart(int i, int j, Rectangle bounds, Color shipCol) {
    float padding = 8.0f;
    float coreX = bounds.x + padding;
    float coreY = bounds.y + padding;
    float coreSize = CELLSIZE - (padding * 2);

    DrawRectangleRounded((Rectangle){coreX, coreY, coreSize, coreSize}, 0.5f, 4, shipCol);

    bool hasUp    = (i > 0) && (PlayerShipMatrix[i-1][j].type != NULL_SHIP);
    bool hasDown  = (i< 9) &&(PlayerShipMatrix[i+1][j].type != NULL_SHIP);
    bool hasLeft  = (j >0)&& (PlayerShipMatrix[i][j-1].type != NULL_SHIP);
    bool hasRight = (j <9) && (PlayerShipMatrix[i][j+1].type != NULL_SHIP);

    if (hasRight) 
        DrawRectangleRec((Rectangle){coreX+coreSize/2, coreY, coreSize/2+ padding+2,coreSize},shipCol);
    if (hasLeft) 
        DrawRectangleRec((Rectangle){bounds.x- 2, coreY, coreSize/2+padding+2, coreSize}, shipCol);
    if (hasDown) 
        DrawRectangleRec((Rectangle){coreX,coreY + coreSize/2, coreSize, coreSize/2+padding + 2}, shipCol);
    if (hasUp)    
        DrawRectangleRec((Rectangle){coreX,bounds.y-2,coreSize,coreSize/2+ padding + 2},shipCol);
}

void drawGrid(GameState gameState){
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
                
                bool isHovered = CheckCollisionPointRec(mousePoint, bounds);

                if (CheckCollisionPointRec(mousePoint, bounds)) {
                    fillCol = DARKGRAY; 

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        printf("Grid %d-Atacat:[%d][%d]\n", k,i, j);
                        if (k == 0) {
                            topGridAttacks[i][j] = 1;
                        }
                    }
                } else {
                    fillCol = BLACK;
                }
    
                    
                drawButton(bounds,fillCol,lineCol);

                //pentru a pune shipurile
                if (gameState == SETUP && k == 1) { 
                    if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        if (PlayerShipMatrix[i][j].type == NULL_SHIP) {
                            PlayerShipMatrix[i][j].type = SHIP_1; 
                        } else {
                            PlayerShipMatrix[i][j].type = NULL_SHIP;
                        }
                        CalculateFleet();
                    }
                }
                //
                //cel care deseneaza shipul si ii pune culoarea corespunzatoare conform calculelelor
                if (PlayerShipMatrix[i][j].type != NULL_SHIP && k == 1) {
                    Color shipColor = GRAY;
                    if (gameState == SETUP && PlayerShipMatrix[i][j].status ==false) 
                        shipColor = RED;
                    drawShipPart(i,j,bounds,shipColor);
                }
                //deseneaza X pe harta inamicului
                if (k == 0) {
                    if (topGridAttacks[i][j] == 1) {
                        drawX(bounds.x, bounds.y, CELLSIZE);
                    } else if (isHovered) {
                        drawTargetMark(bounds.x, bounds.y, CELLSIZE);
                    }
                }
            }
        }
    }
}

void drawSideMenu(void) {

    int baseX = 12;  
    int baseY = 650; 

    DrawRectangle(baseX, baseY, 200, 400, LIGHTGRAY);
    DrawText("FLEET SETUP", baseX + 10, baseY + 10, 20, BLACK);

    

    int currentY = baseY + 50; 

    for (int size = 4; size >= 1; size--) {
        int leftToPlace = shipsNeeded[size] - shipsFound[size];
        
        const char* text = TextFormat("1x%d Ships: %d", size, leftToPlace);
        
        Color textColor = BLACK;
        if (leftToPlace < 0) textColor = RED;   
        if (leftToPlace == 0) textColor = GREEN; 
        
        DrawText(text, baseX + 10, currentY, 20, textColor);
        currentY += 40; 
    }

    if (boardHasErrors) {
        DrawText("ERROR: Ships touching", baseX + 5, currentY + 20, 15, RED);
        DrawText("or invalid shapes!", baseX + 5, currentY + 40, 15, RED);
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

