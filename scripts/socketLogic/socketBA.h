#ifndef SOCKETBA_H
#define SOCKETBA_H

#define SERVER_PORT 31086
#define DEFAULT_IP "192.168.43.1"

//conecteaza la server, returneaza sockfd sau -1 daca esueaza
int connectToServer(const char* ip, int port);

//trimite un mesaj pe socket, returneaza 0 daca ok, -1 daca eroare
int sendMsg(int sockfd, const char* msg);

//primeste mesaj non-blocking, returneaza nr de bytes cititi, 0 daca nimic, -1 daca eroare
int recvMsg(int sockfd, char* buf, int bufSize);

//inchide conexiunea
void closeConnection(int sockfd);

#endif
