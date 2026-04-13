//server.c - standalone battleship server
//ruleaza pe Termux, accepta 2 playeri pe TCP
//usage: ./server

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 31086
#define GRID_SIZE 10
#define TOTAL_SHIP_CELLS 20 // 4*1 + 3*2 + 2*3 + 1*4 = 4+6+6+4 = 20

int boards[2][GRID_SIZE][GRID_SIZE]; //0=apa, 1=ship
int hits[2][GRID_SIZE][GRID_SIZE];   //0=neatacat, 1=atacat
int shipCellsLeft[2];
int clientSock[2];

void sendToClient(int playerIdx, const char* msg) {
    send(clientSock[playerIdx], msg, strlen(msg), 0);
    printf("SERVER: -> P%d: %s", playerIdx + 1, msg);
}

int recvFromClient(int playerIdx, char* buf, int bufSize) {
    int n = recv(clientSock[playerIdx], buf, bufSize - 1, 0);
    if (n <= 0) return n;
    buf[n] = '\0';
    printf("SERVER: <- P%d: %s", playerIdx + 1, buf);
    return n;
}

//asteapta un mesaj specific de la un player
int waitForMsg(int playerIdx, char* buf, int bufSize) {
    while (1) {
        int n = recvFromClient(playerIdx, buf, bufSize);
        if (n <= 0) {
            printf("SERVER: Player %d s-a deconectat\n", playerIdx + 1);
            return -1;
        }
        return n;
    }
}

int parseBoard(int playerIdx, const char* boardStr) {
    int cells = 0;
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        int val = boardStr[i]-'0';
        boards[playerIdx][i/GRID_SIZE][i%GRID_SIZE] = val;
        if (val == 1) cells++;
    }
    shipCellsLeft[playerIdx] = cells;
    printf("SERVER: P%d board primit, %d celule cu ship\n", playerIdx + 1, cells);
    return cells;
}

int main(void) {
    printf("=== BATTLESHIP SERVER ===\n");
    printf("Port: %d\n", PORT);

    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        printf("SERVER: Eroare socket: %s\n", strerror(errno));
        return 1;
    }

    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(serverSock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("SERVER: Eroare bind: %s\n", strerror(errno));
        close(serverSock);
        return 1;
    }

    if (listen(serverSock, 2) < 0) {
        printf("SERVER: Eroare listen: %s\n", strerror(errno));
        close(serverSock);
        return 1;
    }

    printf("SERVER: Astept playeri...\n");

    //acceptam 2 playeri
    int playersConnected[2] = {0, 0};
    char buf[512];

    for (int i = 0; i < 2; i++) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int sock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientLen);
        if (sock < 0) {
            printf("SERVER: Eroare accept: %s\n", strerror(errno));
            close(serverSock);
            return 1;
        }

        //citim HELLO mesajul
        int n = recv(sock, buf, sizeof(buf) - 1, 0);
        if (n <= 0) {
            printf("SERVER: Client deconectat inainte de HELLO\n");
            close(sock);
            i--;
            continue;
        }
        buf[n] = '\0';
        printf("SERVER: <- %s", buf);

        int playerNum = 0;
        if (sscanf(buf, "HELLO %d", &playerNum) != 1 || (playerNum != 1 && playerNum != 2)) {
            printf("SERVER: HELLO invalid, ignoram\n");
            close(sock);
            i--;
            continue;
        }

        int idx = playerNum - 1;
        //daca slotul cerut e ocupat, ii dam cel liber
        if (playersConnected[idx]) {
            int other = 1 - idx;
            if (playersConnected[other]) {
                printf("SERVER: Ambele sloturi ocupate, refuzam\n");
                const char* reject = "REJECT\n";
                send(sock, reject, strlen(reject), 0);
                close(sock);
                i--;
                continue;
            }
            printf("SERVER: Player %d deja conectat, il punem pe Player %d\n", playerNum, other + 1);
            idx = other;
        }

        clientSock[idx] = sock;
        playersConnected[idx] = 1;
        printf("SERVER: Player %d conectat\n", idx + 1);

        char okMsg[32];
        snprintf(okMsg, sizeof(okMsg), "OK %d\n", idx + 1);
        sendToClient(idx, okMsg);
    }

    printf("SERVER: Ambii playeri conectati, asteptam BOARD-urile\n");

    //asteptam BOARD de la ambii playeri
    int boardsReceived[2] = {0, 0};
    while (!boardsReceived[0] || !boardsReceived[1]) {
        for (int i = 0; i < 2; i++) {
            if (boardsReceived[i]) continue;
            
            int n = recv(clientSock[i], buf, sizeof(buf) - 1, MSG_DONTWAIT);
            if (n <= 0) {
                if (n == 0) {
                    printf("SERVER: Player %d deconectat\n", i + 1);
                    goto cleanup;
                }
                continue; //EAGAIN, nu e nimic inca
            }
            buf[n] = '\0';
            printf("SERVER: <- P%d: %s", i + 1, buf);

            if (strncmp(buf, "BOARD ", 6) == 0 && strlen(buf + 6) >= 100) {
                parseBoard(i, buf + 6);
                boardsReceived[i] = 1;
            }
        }
        usleep(10000); //10ms sleep ca sa nu spargem CPU-ul
    }

    printf("SERVER: Ambele board-uri primite, jocul incepe!\n");

    //trimitem READY la ambii
    sendToClient(0, "READY\n");
    sendToClient(1, "READY\n");

    //player 1 incepe mereu primul
    int currentPlayer = 0;
    int gameRunning = 1;

    while (gameRunning) {
        int opponent = 1 - currentPlayer;

        sendToClient(currentPlayer, "TURN\n");
        sendToClient(opponent, "WAIT\n");

        //asteptam ATTACK de la currentPlayer
        if (waitForMsg(currentPlayer, buf, sizeof(buf)) <= 0) {
            printf("SERVER: Player %d deconectat\n", currentPlayer + 1);
            break;
        }

        int row, col;
        if (sscanf(buf, "ATTACK %d %d", &row, &col) != 2) {
            printf("SERVER: ATTACK invalid de la P%d: %s", currentPlayer + 1, buf);
            sendToClient(currentPlayer, "TURN\n"); //retrimitem TURN
            continue;
        }

        if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
            printf("SERVER: Coordonate invalide: %d %d\n", row, col);
            sendToClient(currentPlayer, "TURN\n"); //retrimitem TURN
            continue;
        }

        if (hits[opponent][row][col]) {
            //celula deja atacata, trimitem din nou rezultatul
            if (boards[opponent][row][col]) {
                char resp[64];
                snprintf(resp, sizeof(resp), "HIT %d %d\n", row, col);
                sendToClient(currentPlayer, resp);
                snprintf(resp, sizeof(resp), "YOUHIT %d %d\n", row, col);
                sendToClient(opponent, resp);
            } else {
                char resp[64];
                snprintf(resp, sizeof(resp), "MISS %d %d\n", row, col);
                sendToClient(currentPlayer, resp);
                snprintf(resp, sizeof(resp), "YOUMISS %d %d\n", row, col);
                sendToClient(opponent, resp);
            }
            currentPlayer = opponent;
            continue;
        }

        hits[opponent][row][col] = 1;

        if (boards[opponent][row][col]) {
            //HIT
            shipCellsLeft[opponent]--;
            char resp[64];
            snprintf(resp, sizeof(resp), "HIT %d %d\n", row, col);
            sendToClient(currentPlayer, resp);
            snprintf(resp, sizeof(resp), "YOUHIT %d %d\n", row, col);
            sendToClient(opponent, resp);

            printf("SERVER: P%d a lovit [%d][%d], P%d mai are %d celule\n",
                   currentPlayer + 1, row, col, opponent + 1, shipCellsLeft[opponent]);

            //verificam daca jocul s-a terminat
            if (shipCellsLeft[opponent] <= 0) {
                sendToClient(currentPlayer, "WIN\n");
                sendToClient(opponent, "LOSE\n");
                printf("SERVER: Player %d a castigat!\n", currentPlayer + 1);
                gameRunning = 0;
            }
        } else {
            //MISS
            char resp[64];
            snprintf(resp, sizeof(resp), "MISS %d %d\n", row, col);
            sendToClient(currentPlayer, resp);
            snprintf(resp, sizeof(resp), "YOUMISS %d %d\n", row, col);
            sendToClient(opponent, resp);

            printf("SERVER: P%d a ratat [%d][%d]\n", currentPlayer + 1, row, col);

            //schimbam turul doar la miss
            currentPlayer = opponent;
        }
    }

cleanup:
    printf("SERVER: Jocul s-a terminat, inchidem\n");
    close(clientSock[0]);
    close(clientSock[1]);
    close(serverSock);
    return 0;
}
