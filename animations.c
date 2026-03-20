#include <stdlib.h> 
#include <time.h>   
#include <math.h>

#include "raylib.h"

#define SONAR_PATH "assets/sounds/sonarSound2.mp3"

typedef struct {
    float angle;
    float distance;
    float alpha;
    float radius;
    bool active;
} RadarBlip;

typedef enum { RADAR_ROTATING, RADAR_PAUSED } RadarState;

//da play la animatie cu sunet de la sonarpath de mai sus

void drawRadar(int centerX, int centerY, float maxRadius) {
    static bool initialized = false;
    static RadarState state = RADAR_ROTATING;
    static float angle = -PI/2.0f;
    static float prevAngle = -PI/2.0f;
    static float pauseTimer = 0.0f;

    static Sound sonarSound;

    #define MAX_BLIPS 6
    static RadarBlip randomBlips[MAX_BLIPS];
    static int currentBatch = 0; 
    
    if (!initialized) {
        srand(time(NULL));
        sonarSound = LoadSound(SONAR_PATH);
        PlaySound(sonarSound);
        for (int i = 0; i < MAX_BLIPS; i++) {
            randomBlips[i].active = true; 
            randomBlips[i].alpha = 0.0f;
            randomBlips[i].radius = 0.0f;
        }
        
        for (int i = 0; i < 3; i++) {
            randomBlips[i].active = false;
            randomBlips[i].angle = (((float)rand() / (float)RAND_MAX) * 2.0f * PI) - (PI/2.0f);
            randomBlips[i].distance = ((float)rand() / (float)RAND_MAX) * (maxRadius * 0.8f) + (maxRadius * 0.1f);
        }
        initialized = true;
    }

    float f = 0.5f; 
    float rotationSpeed = 2.0f * PI * f;
    float pauseDuration = 1.0f;
    float dt = GetFrameTime();

    prevAngle = angle;

    switch (state) {
        case RADAR_ROTATING:
            angle += rotationSpeed * dt;
            
            if (angle >= (3.0f * PI / 2.0f)) {
                angle = -PI/2.0f;
                state = RADAR_PAUSED;
                pauseTimer = 0.0f;

                currentBatch = (currentBatch == 0) ? 3 : 0;

                for (int i = 0; i < 3; i++) {
                    int idx = currentBatch + i;
                    randomBlips[idx].active = false; 
                    randomBlips[idx].angle = (((float)rand() / (float)RAND_MAX) * 2.0f * PI) - (PI/2.0f);
                    randomBlips[idx].distance = ((float)rand() / (float)RAND_MAX) * (maxRadius * 0.8f) + (maxRadius * 0.1f);
                }
            }
            break;

        case RADAR_PAUSED:
            pauseTimer += dt;
            if (pauseTimer >= pauseDuration) {
                state = RADAR_ROTATING;
                angle = -PI/2.0f;
                prevAngle = angle;

                PlaySound(sonarSound);
            }
            break;
    }

    for (int i = 0; i < MAX_BLIPS; i++) {
        if (!randomBlips[i].active && prevAngle <= randomBlips[i].angle && angle > randomBlips[i].angle) {
            randomBlips[i].active = true;
            randomBlips[i].alpha = 1.0f;
            randomBlips[i].radius = 12.0f;
        }

        if (randomBlips[i].alpha > 0.0f) {
            randomBlips[i].alpha -= dt * 0.7f; 
            if (randomBlips[i].alpha < 0.0f) randomBlips[i].alpha = 0.0f;

            if (randomBlips[i].radius > 4.0f) {
                randomBlips[i].radius -= dt * 20.0f;
            }
        }
    }

    Color sonarGreen = Fade(GREEN, 0.8f);
    Color sonarDarkGreen = Fade(DARKGREEN, 0.5f);

    float circleThickness = 4.0f;
    int numCircles = 5;
    for (int i = 1; i <= numCircles; i++) {
        float ringRadius = (maxRadius / numCircles) * i;
        DrawRing((Vector2){(float)centerX, (float)centerY}, 
                 ringRadius - circleThickness, ringRadius, 
                 0.0f, 360.0f, 64, sonarDarkGreen);
    }

    for (int i = 0; i < MAX_BLIPS; i++) {
        if (randomBlips[i].alpha > 0.0f) {
            int blipX = centerX + cosf(randomBlips[i].angle) * randomBlips[i].distance;
            int blipY = centerY + sinf(randomBlips[i].angle) * randomBlips[i].distance;
            
            Color currentBlipColor = Fade(GREEN, randomBlips[i].alpha);
            DrawCircle(blipX, blipY, randomBlips[i].radius, currentBlipColor);
        }
    }

    float lineThickness = 6.0f;
    int endX = centerX + cosf(angle) * maxRadius;
    int endY = centerY + sinf(angle) * maxRadius;
    
    DrawLineEx((Vector2){(float)centerX, (float)centerY}, (Vector2){(float)endX, (float)endY}, lineThickness, sonarGreen);
    DrawCircle(centerX, centerY, 6.0f, sonarGreen);
}