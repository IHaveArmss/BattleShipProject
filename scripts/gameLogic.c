#include <stdio.h>
#include <string.h>

#include "gameLogic.h"
#include "socketLogic/socketBA.h"

int topGridAttacks[10][10] = {0};
int bottomGridAttacks[10][10] = {0};
ShipBuild PlayerShipMatrix[10][10];
Tools toolsState = 0;
int shipsNeeded[5] = {0, 4, 3, 2, 1};
int shipsFound[5]  = {0, 0, 0, 0, 0};
bool boardHasErrors = false;
int tempR[100], tempC[100];

//networking globals
int playerNumber = 0;
int sockfd = -1;
bool isConnected = false;
bool isMyTurn = false;
int gameOverResult = 0;
char serverIp[64] = DEFAULT_IP;

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

bool isFleetValid() {
    if (boardHasErrors) return false;
    for (int i = 1; i <= 4; i++) {
        if (shipsFound[i] != shipsNeeded[i]) return false;
    }
    return true;
}

void sendBoard() {
    //flatten PlayerShipMatrix intr-un string de 100 de 0/1
    char boardStr[128];
    char msg[256];
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            boardStr[i * 10 + j] = (PlayerShipMatrix[i][j].type != NULL_SHIP) ? '1' : '0';
        }
    }
    boardStr[100] = '\0';
    snprintf(msg, sizeof(msg), "BOARD %s\n", boardStr);
    sendMsg(sockfd, msg);
}

void sendAttack(int row, int col) {
    char msg[64];
    snprintf(msg, sizeof(msg), "ATTACK %d %d\n", row, col);
    sendMsg(sockfd, msg);
}

void processServerMessage(const char* msg, GameState* state) {
    int row, col;

    if (strncmp(msg, "READY", 5) == 0) {
        printf("GAME: Server zice READY, jocul incepe!\n");
        //starea se va seta cand primim TURN sau WAIT
    }
    else if (strncmp(msg, "TURN", 4) == 0) {
        printf("GAME: E randul nostru!\n");
        isMyTurn = true;
        *state = PLAYER_TURN;
    }
    else if (strncmp(msg, "WAIT", 4) == 0) {
        printf("GAME: Asteptam tura inamicului\n");
        isMyTurn = false;
        *state = ENEMY_TURN;
    }
    else if (sscanf(msg, "HIT %d %d", &row, &col) == 2) {
        printf("GAME: Am lovit la [%d][%d]!\n", row, col);
        topGridAttacks[row][col] = 1;
        isMyTurn = true; //inca o tura la hit
        //marcam diagonalele ca fiind libere
        for (int di = -1; di <= 1; di += 2) {
            for (int dj = -1; dj <= 1; dj += 2) {
                int ni = row + di;
                int nj = col + dj;
                if (ni >= 0 && ni < 10 && nj >= 0 && nj < 10) {
                    if (topGridAttacks[ni][nj] == 0 || topGridAttacks[ni][nj] == 3) {
                        topGridAttacks[ni][nj] = 2;
                    }
                }
            }
        }
    }
    else if (sscanf(msg, "MISS %d %d", &row, &col) == 2) {
        printf("GAME: Am ratat la [%d][%d]\n", row, col);
        topGridAttacks[row][col] = 2;
    }
    else if (sscanf(msg, "YOUHIT %d %d", &row, &col) == 2) {
        printf("GAME: Inamicul ne-a lovit la [%d][%d]\n", row, col);
        bottomGridAttacks[row][col] = 1;
    }
    else if (sscanf(msg, "YOUMISS %d %d", &row, &col) == 2) {
        printf("GAME: Inamicul a ratat la [%d][%d]\n", row, col);
        bottomGridAttacks[row][col] = 2;
    }
    else if (strncmp(msg, "WIN", 3) == 0) {
        printf("GAME: AM CASTIGAT!\n");
        gameOverResult = 1;
        *state = GAME_OVER;
    }
    else if (strncmp(msg, "LOSE", 4) == 0) {
        printf("GAME: AM PIERDUT!\n");
        gameOverResult = 2;
        *state = GAME_OVER;
    }
    else if (strncmp(msg, "OK", 2) == 0) {
        int assignedNum = 0;
        if (sscanf(msg, "OK %d", &assignedNum) == 1 && (assignedNum == 1 || assignedNum == 2)) {
            if (assignedNum != playerNumber) {
                printf("GAME: Serverul ne-a pus Player %d (am cerut %d)\n", assignedNum, playerNumber);
                playerNumber = assignedNum;
            }
        }
        printf("GAME: Server a confirmat conexiunea ca Player %d\n", playerNumber);
    }
}

void resetGameState() {
    memset(topGridAttacks, 0, sizeof(topGridAttacks));
    memset(bottomGridAttacks, 0, sizeof(bottomGridAttacks));
    memset(PlayerShipMatrix, 0, sizeof(PlayerShipMatrix));
    memset(shipsFound, 0, sizeof(shipsFound));
    boardHasErrors = false;
    toolsState = toolFire;
    isMyTurn = false;
    gameOverResult = 0;
    playerNumber = 0;
    isConnected = false;
    if (sockfd >= 0) {
        closeConnection(sockfd);
        sockfd = -1;
    }
}
