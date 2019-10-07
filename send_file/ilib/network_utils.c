#ifndef NETWORK_UTILS
#define NETWORK_UTILS

#include "ilib.h"

int closeSocket(int socketfd) {
    return close(socketfd);
}

int nread(int socketfd, void* buffer, ssize_t readSize) {
	int alreadyRead = 0;
	while (alreadyRead < readSize) {
		int sread = read(socketfd, buffer + alreadyRead, readSize - alreadyRead);
		if (sread < 0) {
			if (errno == EINTR) {
				//nothing happen
			} else {
				return alreadyRead;
			}
		} else {
			if (sread == 0) {
				return alreadyRead;
			}
			alreadyRead += sread;
		}
	}
	return alreadyRead;
}


int nwrite(int socketfd, const void* buffer, ssize_t writeSize) {
	int alreadyWriten = 0;
    while (alreadyWriten < writeSize) {
        int swrite = write(socketfd, buffer + alreadyWriten, writeSize - alreadyWriten);
        if (swrite < 0) {
            if (errno == EINTR) {
                //do nothing 
            } else {
                return alreadyWriten;
            }
        } else {
            if (swrite == 0) {
                return alreadyWriten;
            }
            alreadyWriten += swrite;
        }
    }
    return alreadyWriten;
}

int receiveMessage(int socketFd, void* buffer, ssize_t maxBufferSize) {
    //read header:
    int16_t messageLength;
    int readed = nread(socketFd, &messageLength, sizeof(messageLength));
    if (readed < sizeof(messageLength)) return -1;

    messageLength = ntohs(messageLength);
    if (messageLength > maxBufferSize) {
        printf("Message is larger than buffer");
        return -1;
    }
    //printf("%d\n", messageLength);
    //read content
    return nread(socketFd, buffer, messageLength);
}

int sendMessage(int socketFd, const void* buffer, ssize_t sizeOfMessage) {
    int16_t messageLength = htons((int16_t)sizeOfMessage);
    int writen = nwrite(socketFd, &messageLength, sizeof(messageLength));
    
    if (writen < sizeof(messageLength)) return -1;
    return nwrite(socketFd, buffer, sizeOfMessage);
}

void clearBuffer(void* buffer, ssize_t sizeOfBuffer) {
    memset(buffer, 0, sizeOfBuffer);
}



// void printErrorCode() {
//     printf("error code: %d\n", errno);
// }

// int getPresentationIpv4(const in_addr_t* addr, char* readableAddr) {
//     return inet_ntop(AF_INET, addr, readableAddr, sizeof(readableAddr));
// }

#endif