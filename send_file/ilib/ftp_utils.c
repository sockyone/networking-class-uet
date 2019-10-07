#ifndef CUSTOM_FTP_IMPLEMENT_PACKAGE
#define CUSTOM_FTP_IMPLEMENT_PACKAGE

#include "ilib.h"

#define FTP_COMMAND_PUT 1
#define FTP_COMMAND_GET 2
#define FTP_COMMAND_LIST 3
#define FTP_COMMAND_REMOVE 4
#define FTP_COMMAND_QUIT 5
#define FTP_RESPONSE_WAITFORDOWNLOAD 6
#define FTP_RESPONSE_WAITFORUPLOAD 7
#define FTP_RESPONSE_LIST 8
#define FTP_RESPONSE_REJECT 9
#define FTP_RESPONSE_SUCCESS 10


//local command:
#define FTP_COMMAND_LS 11
#define FTP_COMMAND_RM 12

const int BUFFER_MAX_FILE_READ = 4096;


int ftp_send(int socket, int CODE, const void* buffer, ssize_t writeSize) {
    int16_t messageLength = htons((int16_t)writeSize);
    int16_t code = htons((int16_t)CODE);
    //send messageLength
    int sent = nwrite(socket, &messageLength, sizeof(messageLength));
    if (sent != sizeof(messageLength)) {
        return -1;
    }
    //send code
    sent = nwrite(socket, &code, sizeof(code));
    if (sent != sizeof(code)) {
        return -1;
    }

    sent = nwrite(socket, buffer, writeSize);
    if (sent != writeSize) {
        return -1;
    }

    //return 0 if all success
    return 0;
}

int ftp_send_file(int socket, FILE* fp) {
    char buffer[BUFFER_MAX_FILE_READ];
    rewind(fp);
    while (1) {
        int read = fread(buffer, 1, BUFFER_MAX_FILE_READ, fp);
        //send
        int sent = nwrite(socket, buffer, read);
        if (sent < read) {
            //error
            return -1;
        }
        if (read < BUFFER_MAX_FILE_READ) {
            //read all
            break;
        }
    }
    return 0;
}

int ftp_receive_file(int socket, FILE* fp) {
    char buffer[BUFFER_MAX_FILE_READ];
    while (1) {
        clearBuffer(buffer, BUFFER_MAX_FILE_READ);
        int alreadyRead = read(socket, buffer, BUFFER_MAX_FILE_READ);
        if (alreadyRead < 0) {
            return -1;
        }
        if (alreadyRead == 0) {
            break;
        } 
        fwrite(buffer, 1, alreadyRead, fp);
    }
    return 0;
}

int ftp_receive(int socket, int16_t* CODE, void* buffer, ssize_t maxBufferSize) {

    clearBuffer(buffer, maxBufferSize);
    int16_t messageLength;

    //read size of content
    int read = nread(socket, &messageLength, sizeof(messageLength));
    if (read != sizeof(messageLength)) {
        return -1;
    }
    //if message is too large
    messageLength = ntohs(messageLength);
    if (messageLength > maxBufferSize) {
        return -1;
    }

    //convert

    read = nread(socket, CODE, sizeof(int16_t));
    //read code fail
    if (read != sizeof(int16_t)) {
        return -1;
    }
    //convert
    *CODE = ntohs(*CODE);

    //read into buffer
    read = nread(socket, buffer, messageLength);
    if (read != messageLength) {
        return -1;
    }

    //return 0 if all success
    return 0;
}

#endif