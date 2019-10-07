#ifndef CLIENT_UTILS
#define CLIENT_UTILS

#include "ilib.h"

int getTCPSocketConnectionIpv4(char* ipAddress, int port) {
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    //try to convert ipAddress to binary
    int valid = inet_pton(AF_INET, (const char*) ipAddress, (void *)&servaddr.sin_addr.s_addr);
    if (valid == 0) {
        printf("Not valid ip address\n");
        return -1;
    } else if (valid < 0) {
        printf("Error in convert ip address, code: %d\n", errno);
        return -1;
    }

    //servaddr.sin_addr.s_addr = htonl(servaddr.sin_addr.s_addr);

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        printf("Can not init a socket\n");
        return -1;
    }

    //printf("Socket: %d", socketfd);
    
    //connect
    if (connect(socketfd, (const struct sockaddr*)(&servaddr), sizeof(servaddr)) < 0) {
        printf("Err: %d\n", errno);
        closeSocket(socketfd);
        return -1;
    } else {
        return socketfd;
    }

    return -1;
}

#endif