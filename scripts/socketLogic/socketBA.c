#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "socketBA.h"

int connectToServer(const char* ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("SOCKET: Eroare la creare socket: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serverAddr.sin_addr) <= 0) {
        printf("SOCKET: IP invalid: %s\n", ip);
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("SOCKET: Eroare la conectare: %s\n", strerror(errno));
        close(sockfd);
        return -1;
    }

    //setam non-blocking dupa connect
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    printf("SOCKET: Conectat la %s:%d\n", ip, port);
    return sockfd;
}

int sendMsg(int sockfd, const char* msg) {
    int len = strlen(msg);
    int totalSent = 0;
    while (totalSent < len) {
        int sent = send(sockfd, msg + totalSent, len - totalSent, 0);
        if (sent < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(1000); //asteptam 1ms si incercam din nou
                continue;
            }
            printf("SOCKET: Eroare la trimitere: %s\n", strerror(errno));
            return -1;
        }
        totalSent += sent;
    }
    printf("SOCKET: Trimis -> %s", msg);
    return 0;
}

int recvMsg(int sockfd, char* buf, int bufSize) {
    int received = recv(sockfd, buf, bufSize - 1, 0);
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0; //nimic de citit, e ok
        }
        printf("SOCKET: Eroare la primire: %s\n", strerror(errno));
        return -1;
    }
    if (received == 0) {
        printf("SOCKET: Serverul a inchis conexiunea\n");
        return -1;
    }
    buf[received] = '\0';
    printf("SOCKET: Primit <- %s", buf);
    return received;
}

void closeConnection(int sockfd) {
    if (sockfd >= 0) {
        close(sockfd);
        printf("SOCKET: Conexiune inchisa\n");
    }
}
