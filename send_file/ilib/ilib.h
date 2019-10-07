/* A CUSTOMER LIB FOR NETWORKING PROBLEM SOLVE */

#ifndef CUSTOM_ILIB
#define CUSTOM_ILIB

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>

//for reading dir
#include <dirent.h>

//gethostbyname()
//getsockopt(sockfd, int level, int optname, void* opval, socklen+t *optlen)
/*
    level: SOL_SOCKET: tuy bien chung, IPPROTO_IP, ..: tuy bien rieng
    optname: so nguyuen duong dac ta tuy bien
    optval: luu gia tri cua tuy bien
    optval: length cua optval
*/
/*
    struct hostent {
        char* h_name;
        char** h_aliases;
        int h_addrtype; (AF_INET)
        int h_length; (4)
        char** h_addr_list;
    }
*/

#include "server_utils.c"
#include "network_utils.c"
#include "client_utils.c"
#include "ftp_utils.c"
#include "string_utils.c"


//ftp_utils
int ftp_send(int socket, int CODE, const void* buffer, ssize_t writeSize);
int ftp_receive(int socket, int16_t* CODE, void* buffer, ssize_t maxBufferSize);
int ftp_send_file(int socket, FILE* fp);
int ftp_receive_file(int socket, FILE* fp);

//server utils
int getTCPSocketListenerIpv4(int port, int backlog);
int bindAndListenToSocket(int socketfd, int port, int backlog);
int getTCPSocketIpv4();

//client utils
int getTCPSocketConnectionIpv4(char* ipAddress, int port);

//utils
// void printErrorCode();
//int getPresentationIpv4((const in_addr_t *) addr, char* readableAddr);
int closeSocket();
int nread(int socketfd, void* buffer, ssize_t readSize);
int nwrite(int socketfd, const void* buffer, ssize_t writeSize);
int receiveMessage(int socketFd, void* buffer, ssize_t maxBufferSize);
int sendMessage(int socketFd, const void* buffer, ssize_t sizeOfMessage);
void clearBuffer(void* buffer, ssize_t sizeOfBuffer);


//string utils
void removeLineBreakCharacter(char* string, ssize_t maxSize);
void toLowerCase(char* string, ssize_t maxSize);
int lengthOfString(char* string, ssize_t maxSize);
int checkSpace(char* string, int start, int end, char* buffer, ssize_t maxBufferSize);

#endif
