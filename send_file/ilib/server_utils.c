#ifndef SERVER_UTILS
#define SERVER_UTILS

#include "ilib.h"

int bindAndListenToSocket(int socketfd, int port, int backlog) {
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (bind(socketfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        printf("fail in bind\n");
        return -1;
    }

    if (listen(socketfd, backlog) < 0 ) {
        printf("fail in listen\n");
        return -1;
    }
    return socketfd;
}

int getTCPSocketIpv4() {
    int socketfd = -1;
    socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketfd < 0) {
        printf("fail in create socket\n");
        return -1;
    }
    return socketfd;
}

int getTCPSocketListenerIpv4(int port, int backlog) {
    int socketfd = getTCPSocketIpv4();
    if (socketfd < 0) return -1;
    return bindAndListenToSocket(socketfd, port, backlog);
}


#endif
