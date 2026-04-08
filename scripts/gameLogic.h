#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <stdbool.h>

typedef enum GameState {
    MENU,
    GAMEPLAY,
    SETUP,
    ENEMY_TURN,
    PLAYER_TURN
} GameState;

typedef enum tools {
    toolFire,
    toolMarkMaybe,
    toolClearMark
} Tools;

typedef enum ShipType {
    NULL_SHIP,
    SHIP_1,
    SHIP_2,
    SHIP_3,
    SHIP_4
} ShipType;

typedef struct Ship {
    ShipType type;
    bool status;
} ShipBuild;

extern int topGridAttacks[10][10];
extern ShipBuild PlayerShipMatrix[10][10];
extern ShipBuild EnemyShipMatrix[10][10];
extern Tools toolsState;
extern int shipsNeeded[5];
extern int shipsFound[5];
extern bool boardHasErrors;
extern int tempR[100];
extern int tempC[100];
extern bool enemyShipsInitialized;

void InitEnemyShips();
void FloodFillShip(int i, int j, int* minI, int* maxI, int* minJ, int* maxJ, int* count, bool visited[10][10]);
void CalculateFleet();

#endif
