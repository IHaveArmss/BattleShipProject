#include "raylib.h"
#include <stdio.h>




#define CHECKER_BOARD 0
#define POS_OFFSET_X 150
#define POS_OFFSET_Y 25
#define HEIGHT_PADDING 550
#define CELLSIZE 55

void drawGrid(int typeOfGrid/* 0 for enemy grid 1 for your grid*/){
    if(typeOfGrid == 0){
        for(int i=1;i<=10;i++)
                {
                    for(int j=1;j<=10;j++){
                        if(CHECKER_BOARD==1){
                            Color squareColor;

                            if((i+j) % 2 == 0){
                                squareColor =BLACK;
                            }
                            else{
                                squareColor =RED;
                            }
                            DrawRectangle(CELLSIZE * i, CELLSIZE * j, CELLSIZE, CELLSIZE, squareColor);
                        }
                        else
                            {
                            DrawRectangle(POS_OFFSET_X + j * CELLSIZE, i * CELLSIZE-20, CELLSIZE, CELLSIZE, BLACK);
                            DrawRectangleLines(POS_OFFSET_X + j * CELLSIZE, i * CELLSIZE-20, CELLSIZE, CELLSIZE, GREEN);
                            }
                    }   
                }
    }
    else{
    for(int i=1;i<=10;i++)
            {
                for(int j=1;j<=10;j++){
                    if(CHECKER_BOARD==1){
                        Color squareColor;

                        if((i+j) % 2 == 0){
                            squareColor =BLACK;
                        }
                        else{
                            squareColor =RED;
                        }
                        DrawRectangle(CELLSIZE * i, CELLSIZE * j+HEIGHT_PADDING, CELLSIZE, CELLSIZE, squareColor);
                    }
                    else{
                        DrawRectangle(POS_OFFSET_X + j * CELLSIZE, POS_OFFSET_Y +i * CELLSIZE+HEIGHT_PADDING, CELLSIZE, CELLSIZE, BLACK);
                        DrawRectangleLines(POS_OFFSET_X + j * CELLSIZE, POS_OFFSET_Y +i * CELLSIZE+HEIGHT_PADDING, CELLSIZE, CELLSIZE, GREEN);
                        }
                }   
            }
    } 
}