#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#include "widget.h"
#include "raylib.h"
#include "stdbool.h"
#include "animations.h"
#include "rlgl.h"
#include "socketLogic/socketBA.h"


#define CHECKER_BOARD 0
#define BASE_SCREEN_HEIGHT 1200.0f
#define BASE_POS_OFFSET_X 225.0f
#define BASE_TOP_MARGIN 30.0f
#define BASE_HEIGHT_PADDING 600.0f
#define BASE_CELLSIZE 55.0f
#define DARKRED CLITERAL(Color){ 220, 20, 60, 255 }
#define CHECKERBOARD



static bool ipFieldActive = false;

static int ScaleUi(float value) {
    float scale = (float)GetScreenHeight()/BASE_SCREEN_HEIGHT;
    if (scale < 0.75f) scale = 0.75f;
    return (int)roundf(value*scale);
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

void drawGrayDot(int posx, int posy, int size) {
    int centerX = posx + size / 2;
    int centerY = posy + size / 2;
    DrawCircle(centerX, centerY, 4, GRAY);
}

void drawQuestionMark(int posx, int posy, int size) {
    int fontSize = (int)(size * 0.65f);
    const char* text = "?";
    int textW = MeasureText(text, fontSize);
    int textX = posx + (size - textW) / 2;
    int textY = posy + (size - fontSize) / 2;
    DrawText(text, textX, textY, fontSize, ORANGE);
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

    //label-uri pentru griduri
    int labelSize = ScaleUi(18.0f);
    const char* topLabel = "ENEMY GRID";
    const char* botLabel = "YOUR GRID";
    int topLabelX = posOffsetX + (gridWidth - MeasureText(topLabel, labelSize))/2;
    int botLabelX = posOffsetX + (gridWidth - MeasureText(botLabel, labelSize))/2;
    DrawText(topLabel, topLabelX, baseY + topMargin - labelSize - 4, labelSize, GREEN);
    DrawText(botLabel, botLabelX, baseY + topMargin + gridHeight + interGridGap - labelSize - 4, labelSize, GREEN);

    //tura indicator
    if (gameState == PLAYER_TURN) {
        const char* turnText = "YOUR TURN - FIRE!";
        int turnSize = ScaleUi(22.0f);
        int turnX = (screenW - MeasureText(turnText, turnSize))/2;
        DrawText(turnText, turnX, ScaleUi(5.0f), turnSize, GREEN);
    } else if (gameState == ENEMY_TURN) {
        const char* turnText = "ENEMY'S TURN - WAIT...";
        int turnSize = ScaleUi(22.0f);
        int turnX = (screenW - MeasureText(turnText, turnSize))/2;
        DrawText(turnText, turnX, ScaleUi(5.0f), turnSize, RED);
    }

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
                        //atacuri pe gridu inamic - doar in PLAYER_TURN
                        if (k == 0 && gameState == PLAYER_TURN && isMyTurn) {
                            if (toolsState == toolFire && topGridAttacks[i][j] == 0) {
                                sendAttack(i, j);
                                isMyTurn = false; //asteptam raspunsul serverului
                            }
                            else if(toolsState == toolMarkMaybe && topGridAttacks[i][j] == 0){
                                topGridAttacks[i][j] = 3;
                                printf("Grid %d-Marcat:[%d][%d]\n", k,i, j);
                            }
                            else if(toolsState == toolClearMark && topGridAttacks[i][j] == 3){
                                printf("Grid %d-Cleared:[%d][%d]\n", k,i, j);
                                topGridAttacks[i][j] = 0;
                            }
                        }
                        //marcari in orice tura (mark/clear)
                        else if (k == 0 && (gameState == PLAYER_TURN || gameState == ENEMY_TURN)) {
                            if(toolsState == toolMarkMaybe && topGridAttacks[i][j] == 0){
                                topGridAttacks[i][j] = 3;
                            }
                            else if(toolsState == toolClearMark && topGridAttacks[i][j] == 3){
                                topGridAttacks[i][j] = 0;
                            }
                        }
                    }
                }
                 else {
                    fillCol = BLACK;
                }
    
                    
                drawButton(bounds,fillCol,lineCol);

                //pentru a pune shipurile in SETUP
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

                //deseneaza marcaj pe gridul inamicului  
                if (k == 0) {
                    if (topGridAttacks[i][j] == 1) {
                        drawX(bounds.x, bounds.y, cellSize);
                    } else if (topGridAttacks[i][j] == 2) {
                        drawGrayDot(bounds.x, bounds.y, cellSize);
                    } else if (topGridAttacks[i][j] == 3) {
                        drawQuestionMark(bounds.x, bounds.y, cellSize);
                    } else if (isHovered && gameState == PLAYER_TURN && isMyTurn && toolsState == toolFire) {
                        drawTargetMark(bounds.x, bounds.y, cellSize);
                    }
                }

                //deseneaza atacurile inamicului pe gridul nostru (bottom grid)
                if (k == 1 && (gameState == PLAYER_TURN || gameState == ENEMY_TURN || gameState == GAME_OVER)) {
                    if (bottomGridAttacks[i][j] == 1) {
                        drawX(bounds.x, bounds.y, cellSize);
                    } else if (bottomGridAttacks[i][j] == 2) {
                        drawGrayDot(bounds.x, bounds.y, cellSize);
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

bool drawReadyButton(void) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    bool fleetOk = isFleetValid();

    Rectangle btnRect = {
        (float)(screenW - ScaleUi(220.0f)),
        (float)(screenH - ScaleUi(80.0f)),
        (float)ScaleUi(200.0f),
        (float)ScaleUi(60.0f)
    };

    Color btnColor = fleetOk ? DARKGREEN : DARKGRAY;
    Color borderColor = fleetOk ? GREEN : GRAY;

    bool clicked = false;
    if (fleetOk) {
        clicked = drawButton(btnRect, btnColor, borderColor);
    } else {
        drawButton(btnRect, btnColor, borderColor);
    }

    const char* text = fleetOk ? "READY" : "PLACE SHIPS";
    int fontSize = ScaleUi(25.0f);
    int textWidth = MeasureText(text, fontSize);
    int textX = btnRect.x + (btnRect.width - textWidth) / 2;
    int textY = btnRect.y + (btnRect.height - fontSize) / 2;
    DrawText(text, textX, textY, fontSize, WHITE);

    return clicked;
}

int drawPlayerSelect() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    ClearBackground(BACKGROUND_COLOR_MENU);

    //titlu
    int titleSize = ScaleUi(60.0f);
    const char* title = "SELECT PLAYER";
    int titleX = (screenW - MeasureText(title, titleSize))/2;
    DrawText(title, titleX, ScaleUi(80.0f), titleSize, WHITE);

    //IP input field
    int ipLabelSize = ScaleUi(20.0f);
    DrawText("Server IP:", screenW/2 - ScaleUi(150.0f), ScaleUi(200.0f), ipLabelSize, LIGHTGRAY);

    Rectangle ipBox = {
        (float)(screenW/2 - ScaleUi(150.0f)),
        (float)ScaleUi(230.0f),
        (float)ScaleUi(300.0f),
        (float)ScaleUi(40.0f)
    };

    //click pe ip box activeaza editarea
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mp = GetMousePosition();
        ipFieldActive = CheckCollisionPointRec(mp, ipBox);
    }

    Color ipBorderCol = ipFieldActive ? GREEN : GRAY;
    DrawRectangleRec(ipBox, DARKGRAY);
    DrawRectangleLinesEx(ipBox, 2, ipBorderCol);

    //input handling
    if (ipFieldActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= '0' && key <= '9') || key == '.') {
                int len = strlen(serverIp);
                if (len < 63) {
                    serverIp[len] = (char)key;
                    serverIp[len + 1] = '\0';
                }
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(serverIp);
            if (len > 0) serverIp[len - 1] = '\0';
        }
    }

    int ipFontSize = ScaleUi(20.0f);
    DrawText(serverIp, ipBox.x + ScaleUi(10.0f), ipBox.y + (ipBox.height - ipFontSize)/2, ipFontSize, WHITE);

    //blinking cursor
    if (ipFieldActive && ((int)(GetTime()*2) % 2 == 0)) {
        int cursorX = ipBox.x + ScaleUi(10.0f) + MeasureText(serverIp, ipFontSize);
        DrawText("|", cursorX, ipBox.y + (ipBox.height - ipFontSize)/2, ipFontSize, GREEN);
    }

    //butoane player 1 si player 2
    int btnW = ScaleUi(250.0f);
    int btnH = ScaleUi(120.0f);
    int gap = ScaleUi(60.0f);
    int totalW = 2*btnW + gap;
    int startX = (screenW - totalW)/2;
    int btnY = screenH/2 - btnH/2 + ScaleUi(40.0f);

    Rectangle btn1 = { (float)startX, (float)btnY, (float)btnW, (float)btnH };
    Rectangle btn2 = { (float)(startX + btnW + gap), (float)btnY, (float)btnW, (float)btnH };

    bool clicked1 = drawButton(btn1, BLACK, GREEN);
    bool clicked2 = drawButton(btn2, BLACK, GREEN);

    int fontSize = ScaleUi(35.0f);

    const char* p1Text = "PLAYER 1";
    int p1X = btn1.x + (btn1.width - MeasureText(p1Text, fontSize))/2;
    int p1Y = btn1.y + (btn1.height - fontSize)/2;
    DrawText(p1Text, p1X, p1Y, fontSize, GREEN);

    const char* p2Text = "PLAYER 2";
    int p2X = btn2.x + (btn2.width - MeasureText(p2Text, fontSize))/2;
    int p2Y = btn2.y + (btn2.height - fontSize)/2;
    DrawText(p2Text, p2X, p2Y, fontSize, GREEN);

    //sub text
    int subSize = ScaleUi(14.0f);
    const char* subText = "Player 1 attacks first";
    int subX = (screenW - MeasureText(subText, subSize))/2;
    DrawText(subText, subX, btnY + btnH + ScaleUi(20.0f), subSize, GRAY);

    if (clicked1) return 1;
    if (clicked2) return 2;
    return 0;
}

void drawConnecting() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    ClearBackground(BACKGROUND_COLOR_MENU);

    int fontSize = ScaleUi(40.0f);
    const char* text = "CONNECTING...";
    int textX = (screenW - MeasureText(text, fontSize))/2;
    int textY = screenH/2 - fontSize/2;
    DrawText(text, textX, textY, fontSize, GREEN);

    //animatie dots
    int dots = ((int)(GetTime()*2)) % 4;
    const char* dotStr[] = {"", ".", "..", "..."};
    int dotSize = ScaleUi(40.0f);
    int dotX = textX + MeasureText(text, fontSize);
    DrawText(dotStr[dots], dotX, textY, dotSize, GREEN);

    int subSize = ScaleUi(18.0f);
    const char* subText = TextFormat("IP: %s  Port: %d", serverIp, SERVER_PORT);
    int subX = (screenW - MeasureText(subText, subSize))/2;
    DrawText(subText, subX, textY + fontSize + ScaleUi(20.0f), subSize, GRAY);
}

void drawWaitingReady() {
    int screenW = GetScreenWidth();

    int fontSize = ScaleUi(30.0f);
    const char* text = "WAITING FOR OPPONENT...";
    int textX = (screenW - MeasureText(text, fontSize))/2;
    int textY = ScaleUi(10.0f);

    //animatie pulsata
    float alpha = (sinf(GetTime() * 3.0f) + 1.0f) / 2.0f;
    Color col = Fade(GREEN, 0.4f + alpha * 0.6f);
    DrawText(text, textX, textY, fontSize, col);
}

void drawGameOver() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    ClearBackground(BACKGROUND_COLOR_MENU);

    int titleSize = ScaleUi(70.0f);
    const char* title;
    Color titleCol;

    if (gameOverResult == 1) {
        title = TextFormat("PLAYER %d HAS WON", playerNumber);
        titleCol = GREEN;
    } else {
        int opponent = (playerNumber == 1) ? 2 : 1;
        title = TextFormat("PLAYER %d HAS WON", opponent);
        titleCol = RED;
    }

    int titleX = (screenW - MeasureText(title, titleSize))/2;
    int titleY = screenH/2 - titleSize;
    DrawText(title, titleX, titleY, titleSize, titleCol);

    //sub mesaj
    int subSize = ScaleUi(25.0f);
    const char* subText;
    if (gameOverResult == 1) {
        subText = "VICTORY!";
    } else {
        subText = "DEFEAT!";
    }
    int subX = (screenW - MeasureText(subText, subSize))/2;
    DrawText(subText, subX, titleY + titleSize + ScaleUi(20.0f), subSize, titleCol);

    //quit hint
    int hintSize = ScaleUi(16.0f);
    const char* hint = "Close window to exit";
    int hintX = (screenW - MeasureText(hint, hintSize))/2;
    DrawText(hint, hintX, screenH - ScaleUi(60.0f), hintSize, GRAY);
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
