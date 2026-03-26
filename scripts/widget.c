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
#define BASE_SCREEN_HEIGHT 1200.0f
#define BASE_POS_OFFSET_X 225.0f
#define BASE_TOP_MARGIN 30.0f
#define BASE_HEIGHT_PADDING 600.0f
#define BASE_CELLSIZE 55.0f
#define DARKRED CLITERAL(Color){ 220, 20, 60, 255 }
#define CHECKERBOARD


typedef enum tools{
    toolFire,
    toolMarkMaybe,
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


int topGridAttacks[10][10] = {0};// 1 pentru toolfire 2 pt toolmark maybe 3 pt tool clearMark
ShipBuild PlayerShipMatrix[10][10] ;
Tools toolsState = 0;
int shipsNeeded[5] = {0, 4, 3, 2, 1};
int shipsFound[5]  = {0, 0, 0, 0, 0};
bool boardHasErrors = false;
int tempR[100], tempC[100];

static int ScaleUi(float value) {
    float scale = (float)GetScreenHeight()/BASE_SCREEN_HEIGHT;
    if (scale < 0.75f) scale = 0.75f;
    return (int)roundf(value*scale);
}

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

void drawShipPart(int i, int j, Rectangle bounds, Color shipCol, int cellSize) {
    float padding = 8.0f;
    float coreX = bounds.x + padding;
    float coreY = bounds.y + padding;
    float coreSize = cellSize - (padding * 2);

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
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int topMargin = ScaleUi(BASE_TOP_MARGIN);
    int interGridGap = ScaleUi(40.0f);

    int maxCellByScale = ScaleUi(BASE_CELLSIZE);
    int maxCellByHeight = (screenH - (2*topMargin) - interGridGap)/20;
    int cellSize = maxCellByScale;
    if (maxCellByHeight < cellSize) 
        cellSize = maxCellByHeight;
    if (cellSize < 30) 
        cellSize = 30;

    int gridWidth = 10*cellSize;
    int gridHeight = 10*cellSize;
    int fullHeight = (2*gridHeight) + interGridGap;
    int baseY = (screenH - fullHeight)/2;
    if (baseY < 0) 
        baseY = 0;

    int posOffsetX = (screenW - gridWidth)/2;
    if (posOffsetX < 0) 
        posOffsetX = 0;

    Color lineCol = GREEN;
    Color fillCol = BLACK;

    for(int k=0;k<=1;k++){
        int yOffset =0;
        if( k ==1){
            yOffset = gridHeight + interGridGap;
        }
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {

                int posX = posOffsetX + j * cellSize;
                int posY = baseY + yOffset + i * cellSize;
                Rectangle bounds = {posX, posY + topMargin, cellSize, cellSize};
                
                bool isHovered = CheckCollisionPointRec(mousePoint, bounds);

                if (CheckCollisionPointRec(mousePoint, bounds)) {
                    fillCol = DARKGRAY; 

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        if (k == 0 && toolsState == toolFire) {
                            topGridAttacks[i][j] = 1;
                            printf("Grid %d-Atacat:[%d][%d]\n", k,i, j);
                        }
                        else if(k == 0 && toolsState == toolMarkMaybe){
                            topGridAttacks[i][j] = 2;
                            printf("Grid %d-Marcat:[%d][%d]\n", k,i, j);
                        }
                        else if(k == 0 && toolsState == toolClearMark &&topGridAttacks[i][j] == 2){
                            printf("Grid %d-Cleared:[%d][%d]\n", k,i, j);
                            topGridAttacks[i][j] = 0;
                        }
                    }
                }
                 else {
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
                    drawShipPart(i,j,bounds,shipColor,cellSize);
                }
                //deseneaza X pe harta inamicului
                if (k == 0) {
                    if (topGridAttacks[i][j] == 1) {
                        drawX(bounds.x, bounds.y, cellSize);
                    } else if (isHovered&& toolsState == toolFire) {
                        drawTargetMark(bounds.x, bounds.y, cellSize);
                    }
                }
            }
        }
    }
}

void drawSideMenu(void) {

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    int menuWidth = ScaleUi(200.0f);
    int menuHeight = ScaleUi(400.0f);
    int titleSize = ScaleUi(20.0f);
    int bodySize = ScaleUi(20.0f);
    int bodyStep = ScaleUi(40.0f);
    int errorSize = ScaleUi(15.0f);
    int baseX = ScaleUi(12.0f);
    int baseY = screenH - menuHeight - ScaleUi(20.0f);

    if (baseY < ScaleUi(12.0f)) baseY = ScaleUi(12.0f);
    if (baseX + menuWidth > screenW) {
        baseX = screenW - menuWidth - ScaleUi(12.0f);
        if (baseX < 0) baseX = 0;
    }

    DrawRectangle(baseX, baseY, menuWidth, menuHeight, LIGHTGRAY);
    DrawText("FLEET SETUP", baseX + ScaleUi(10.0f), baseY + ScaleUi(10.0f), titleSize, BLACK);

    

    int currentY = baseY + ScaleUi(50.0f);

    for (int size = 4; size >= 1; size--) {
        int leftToPlace = shipsNeeded[size] - shipsFound[size];
        
        const char* text = TextFormat("1x%d Ships: %d", size, leftToPlace);
        
        Color textColor = BLACK;
        if (leftToPlace < 0) textColor = RED;   
        if (leftToPlace == 0) textColor = GREEN; 
        
        DrawText(text, baseX + ScaleUi(10.0f), currentY, bodySize, textColor);
        currentY += bodyStep;
    }

    if (boardHasErrors) {
        DrawText("ERROR: Ships touching", baseX + ScaleUi(5.0f), currentY + ScaleUi(20.0f), errorSize, RED);
        DrawText("or invalid shapes!", baseX + ScaleUi(5.0f), currentY + ScaleUi(40.0f), errorSize, RED);
    }
}
void drawSideTools(void) {
    Rectangle bounds = {50, 100, 100, 150};
    DrawRectangleRec(bounds, LIGHTGRAY);

    float segmentHeight = bounds.height / 3.0f;
    float fontSize = ScaleUi(15.0f);
    Font font = GetFontDefault();
    int clicked;

    // FIRE
    Rectangle bounds1 = { bounds.x, bounds.y, bounds.width, segmentHeight };
    clicked = drawButton(bounds1, BLACK, LIGHTGRAY);
    Vector2 size1 = MeasureTextEx(font, "FIRE", fontSize, 1);
    DrawTextEx(font, "FIRE", (Vector2){ bounds1.x + (bounds1.width - size1.x) / 2, bounds1.y + (bounds1.height - size1.y) / 2 }, fontSize, 1, WHITE);
    if(clicked){
        toolsState = toolFire;
        printf("Tool selected: FIRE\n");
    }
    // UNKNOWN
    Rectangle bounds2 = { bounds.x, bounds.y + segmentHeight, bounds.width, segmentHeight };
    clicked = drawButton(bounds2, BLACK, LIGHTGRAY);
    Vector2 size2 = MeasureTextEx(font, "UNKNOWN", fontSize, 1);
    DrawTextEx(font, "UNKNOWN", (Vector2){ bounds2.x + (bounds2.width - size2.x) / 2, bounds2.y + (bounds2.height - size2.y) / 2 }, fontSize, 1, WHITE);
    if(clicked){
        toolsState = toolMarkMaybe;
        printf("Tool selected: UNKNOWN\n");
    }

    // CLEAR
    Rectangle bounds3 = { bounds.x, bounds.y + 2 * segmentHeight, bounds.width, segmentHeight };
    clicked = drawButton(bounds3, BLACK, LIGHTGRAY);
    Vector2 size3 = MeasureTextEx(font, "CLEAR", fontSize, 1);
    DrawTextEx(font, "CLEAR", (Vector2){ bounds3.x + (bounds3.width - size3.x) / 2, bounds3.y + (bounds3.height - size3.y) / 2 }, fontSize, 1, WHITE);
    if(clicked){
        toolsState = toolClearMark;
        printf("Tool selected: CLEAR\n");
    }
}


bool drawMainMenu(void){
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int titleFontSize = ScaleUi(100.0f);
    int titleY = ScaleUi(150.0f);

    ClearBackground(BACKGROUND_COLOR_MENU);

    int titleX = (screenW - MeasureText("BATTLESHIP", titleFontSize))/2;
    DrawText("BATTLESHIP", titleX, titleY, titleFontSize, WHITE);
    
    drawRadar(screenW / 2, screenH / 2, (float)ScaleUi(200.0f));

    Rectangle btnRect = {
        (float)(screenW/2 - ScaleUi(150.0f)),
        (float)(screenH - ScaleUi(180.0f)),
        (float)ScaleUi(300.0f),
        (float)ScaleUi(100.0f)
    };

    if (btnRect.y + btnRect.height > screenH - ScaleUi(20.0f)) {
        btnRect.y = (float)(screenH - ScaleUi(20.0f) - btnRect.height);
    }
    bool isStartClicked = drawButton(btnRect, BLACK, GREEN);

    const char* text = "START";
    int fontSize = ScaleUi(50.0f);
    int textWidth = MeasureText(text, fontSize);
    int textX = btnRect.x + (btnRect.width - textWidth) / 2;
    int textY = btnRect.y + (btnRect.height - fontSize) / 2;
    DrawText(text, textX, textY, fontSize, WHITE);

    return isStartClicked;
}

