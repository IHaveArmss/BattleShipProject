#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <stdbool.h>

typedef enum GameState {
    MENU,
    SELECT_PLAYER,
    CONNECTING,
    SETUP,
    WAITING_READY,
    PLAYER_TURN,
    ENEMY_TURN,
    GAME_OVER
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

//matricile de joc
extern int topGridAttacks[10][10];     //atacurile noastre pe gridu inamic (0=gol,1=hit,2=miss,3=mark)
extern int bottomGridAttacks[10][10];  //atacurile inamicului pe gridu nostru
extern ShipBuild PlayerShipMatrix[10][10];
extern Tools toolsState;
extern int shipsNeeded[5];
extern int shipsFound[5];
extern bool boardHasErrors;
extern int tempR[100];
extern int tempC[100];

//networking globals
extern int playerNumber;     //1 sau 2
extern int sockfd;           //socket-ul catre server
extern bool isConnected;     
extern bool isMyTurn;
extern int gameOverResult;   //0=nimic, 1=castigat, 2=pierdut
extern char serverIp[64];

//functii de logica
void FloodFillShip(int i, int j, int* minI, int* maxI, int* minJ, int* maxJ, int* count, bool visited[10][10]);
void CalculateFleet();
bool isFleetValid();
void sendBoard();
void sendAttack(int row, int col);
void processServerMessage(const char* msg, GameState* state);
void resetGameState();

#endif
