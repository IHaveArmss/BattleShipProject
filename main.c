#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "scripts/gameLogic.h"
#include "scripts/widget.h"
#include "scripts/socketLogic/socketBA.h"

//10x10 grid, 4 1x1 3 1x2 2 1x3 1 1x4


int main(void) {

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BattleShip");

    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);

    int targetWidth = SCREEN_WIDTH;
    int targetHeight = SCREEN_HEIGHT;

    int horizontalMargin = 80;
    int verticalMargin = 80;

    if (targetWidth > monitorWidth - horizontalMargin) {
        targetWidth = monitorWidth - horizontalMargin;
    }
    if (targetHeight > monitorHeight - verticalMargin) {
        targetHeight = monitorHeight - verticalMargin;
    }

    if (targetWidth < 900) targetWidth = 900;
    if (targetHeight < 700) targetHeight = 700;

    SetWindowSize(targetWidth, targetHeight);

    InitAudioDevice();
    
    SetTargetFPS(60);

    GameState currentState = MENU;
    bool boardSent = false;
    bool connectAttempted = false;
    int connectFrameDelay = 0;

    char recvBuf[512];
    
    while (!WindowShouldClose()) {

        //procesam mesajele de la server 
        if (isConnected && sockfd >= 0) {
            int n = recvMsg(sockfd, recvBuf, sizeof(recvBuf));
            if (n > 0) {
                //poate veni mai mult de un mesaj le procesam pe rand
                char* line = strtok(recvBuf, "\n");
                while (line != NULL) {
                    processServerMessage(line, &currentState);
                    line = strtok(NULL, "\n");
                }
            } else if (n < 0) {
                //server deconectat
                printf("MAIN: Serverul s-a deconectat\n");
                isConnected = false;
            }
        }

        BeginDrawing();

            if(currentState == MENU){
                if(drawMainMenu()){
                    currentState = SELECT_PLAYER;
                }
            }
            else if (currentState == SELECT_PLAYER) {
                int selected = drawPlayerSelect();
                if (selected > 0) {
                    playerNumber = selected;
                    currentState = CONNECTING;
                    connectAttempted = false;
                    connectFrameDelay = 5; //asteptam cateva frame-uri ca sa arate ecranul
                    printf("MAIN: Selectat Player %d\n", playerNumber);
                }
            }
            else if (currentState == CONNECTING) {
                drawConnecting();

                if (connectFrameDelay > 0) {
                    connectFrameDelay--;
                } else if (!connectAttempted) {
                    connectAttempted = true;
                    sockfd = connectToServer(serverIp, SERVER_PORT);
                    if (sockfd >= 0) {
                        isConnected = true;
                        //trimitem HELLO
                        char hello[32];
                        snprintf(hello, sizeof(hello), "HELLO %d\n", playerNumber);
                        sendMsg(sockfd, hello);
                        currentState = SETUP;
                        boardSent = false;
                        printf("MAIN: Conectat, trecem la SETUP\n");
                    } else {
                        printf("MAIN: Eroare la conectare, revenim la SELECT_PLAYER\n");
                        currentState = SELECT_PLAYER;
                    }
                }
            }
            else if (currentState == SETUP) {
                ClearBackground(BACKGROUND_COLOR);
                drawGrid(currentState);
                drawSideMenu();
                drawSideTools();
                
                if (drawReadyButton() && !boardSent) {
                    sendBoard();
                    boardSent = true;
                    currentState = WAITING_READY;
                    printf("MAIN: Board trimis, asteptam adversarul\n");
                }
            }
            else if (currentState == WAITING_READY) {
                ClearBackground(BACKGROUND_COLOR);
                drawGrid(currentState);
                drawWaitingReady();
            }
            else if (currentState == PLAYER_TURN || currentState == ENEMY_TURN) {
                ClearBackground(BACKGROUND_COLOR);
                drawGrid(currentState);
                drawSideTools();
            }
            else if (currentState == GAME_OVER) {
                drawGameOver();
            }
        

        EndDrawing();
    }

    //cleanup
    if (isConnected && sockfd >= 0) {
        closeConnection(sockfd);
    }
    CloseWindow();
    return 0;
}