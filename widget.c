#include <stdio.h>
#include <math.h>

#include "widget.h"
#include "raylib.h"
#include "stdbool.h"



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




typedef enum { RADAR_ROTATING, RADAR_PAUSED } RadarState;

void drawRadar(int centerX, int centerY, float maxRadius) {
    static float angle = -PI/2.0f; 
    static RadarState state = RADAR_ROTATING;
    static float pauseTimer = 0.0f;
    
    // --- setari de viteza
    float f = 1.0f; // frecventa rotati pe secunda
    float rotationSpeed = 2.0f * PI * f; 
    // ---------------------

    float pauseDuration = 1.0f; 
    float pulsationAmount = 0.10f; 
    float dt = GetFrameTime(); 

    // 1. Masina de stari (e automat iapa style))
    switch (state) {
        case RADAR_ROTATING:
            angle += rotationSpeed * dt;
            if (angle >= (3.0f * PI / 2.0f)) { 
                angle = -PI/2.0f; 
                state = RADAR_PAUSED;
                pauseTimer = 0.0f;
            }
            break;

        case RADAR_PAUSED:
            pauseTimer += dt;
            if (pauseTimer >= pauseDuration) {
                state = RADAR_ROTATING;
            }
            break;
    }

    // 2. Logica de pulsatie thank you random website :pray:
    float pulseFactor = 1.0f; 
    
    if (state == RADAR_ROTATING) {
        float rotationTraveled = angle - (-PI/2.0f);
        
        if (rotationTraveled < PI) {
            pulseFactor += pulsationAmount * sinf(rotationTraveled);
        }
    }
    //-----
    float currentRadius = maxRadius * pulseFactor;

    Color sonarGreen = Fade(GREEN, 0.8f);
    Color sonarDarkGreen = Fade(DARKGREEN, 0.5f);

    //3. Desenarea
    float circleThickness = 4.0f; // grosime inele
    float lineThickness = 6.0f;   // grosimea liniei
    // -----------------------------------

    int numCircles = 5;
    for (int i = 1; i <= numCircles; i++) {
        float ringRadius = (currentRadius / numCircles) * i;
        
        DrawRing((Vector2){(float)centerX, (float)centerY}, 
                 ringRadius - circleThickness, ringRadius, 
                 0.0f, 360.0f, 64, sonarDarkGreen);
    }

    int endX = centerX + cosf(angle) * currentRadius;
    int endY = centerY + sinf(angle) * currentRadius;
    
    // FOLOSIM lineThickness în loc de 3.0f
    DrawLineEx((Vector2){(float)centerX, (float)centerY}, (Vector2){(float)endX, (float)endY}, lineThickness, sonarGreen);
    DrawCircle(centerX, centerY, 6.0f, sonarGreen);
}