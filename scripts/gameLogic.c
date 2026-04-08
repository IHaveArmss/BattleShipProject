#include "gameLogic.h"

int topGridAttacks[10][10] = {0};
ShipBuild PlayerShipMatrix[10][10];
ShipBuild EnemyShipMatrix[10][10];
Tools toolsState = 0;
int shipsNeeded[5] = {0, 4, 3, 2, 1};
int shipsFound[5]  = {0, 0, 0, 0, 0};
bool boardHasErrors = false;
int tempR[100], tempC[100];
bool enemyShipsInitialized = false;

void InitEnemyShips() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            EnemyShipMatrix[i][j].type = NULL_SHIP;
            EnemyShipMatrix[i][j].status = true;
        }
    }

    // 1x4 ship at (0,0)
    for(int j=0; j<4; j++) EnemyShipMatrix[0][j].type = SHIP_4;
    // 1x3 ship at (2,2)
    for(int i=2; i<5; i++) EnemyShipMatrix[i][2].type = SHIP_3;
    // 1x2 ship at (7,7)
    for(int j=7; j<9; j++) EnemyShipMatrix[7][j].type = SHIP_2;
    // 1x1 ships
    EnemyShipMatrix[9][0].type = SHIP_1;
    EnemyShipMatrix[9][9].type = SHIP_1;
    EnemyShipMatrix[5][5].type = SHIP_1;
    EnemyShipMatrix[1][7].type = SHIP_1;

    enemyShipsInitialized = true;
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
