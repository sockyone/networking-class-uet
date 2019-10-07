/*
    PHAN PHUONG NAM - 17020918
    CACH SU DUNG SERVER: ./server
    SERVER se chay tai port 3003
*/

#include "ilib/ilib.h"

const int APP_PORT = 3003;
const int FILE_TRANSFER_PORT = 3004;
const int BUFFER_MAX_SIZE = 1024;
const int BACK_LOG_DEFAULT = 15;
const char ROOT_PATH[] = "data/";

int serve_command_list(int socket);
int serve_command_get(int socket, int fileSocket, char* buffer);
int serve_command_put(int socket, int fileSocket, char* buffer);
int serve_command_remove(int socket, char* buffer);
int check_if_exist_file(char* fileName);

int main() {

    int socket = getTCPSocketIpv4();
    int file_socket = getTCPSocketIpv4();

    //set SO_REUSEADDR
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed!\n");
    
    if (setsockopt(file_socket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed!\n");
    
    int rcvBufferSize = 0;

    printf("Welcome to fake FTP server. Please init your config\n");
    printf("Insert receive buffer size (byte): ");
    scanf("%d", &rcvBufferSize);

    if (rcvBufferSize > 0) {
        if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &rcvBufferSize, sizeof(rcvBufferSize)) < 0) {
            perror("setsockopt(SO_RCVBUF) failed\n");
        }
        if (setsockopt(file_socket, SOL_SOCKET, SO_RCVBUF, &rcvBufferSize, sizeof(rcvBufferSize)) < 0) {
            perror("setsockopt(SO_RCVBUF) failed\n");
        }
    }

    socket = bindAndListenToSocket(socket, APP_PORT, BACK_LOG_DEFAULT);
    file_socket = bindAndListenToSocket(file_socket, FILE_TRANSFER_PORT, BACK_LOG_DEFAULT);

    if (socket < 0) {
        printf("Failed to start server!\n");
        return 1;
    }
    
    printf("Server start listening in port %d\n", APP_PORT);

    //logic of server
    while (1) {
        struct sockaddr_in clientAddr;
        clearBuffer(&clientAddr, sizeof(clientAddr));
        socklen_t clientAddrSize = sizeof(clientAddr);

        //accept the connection
        int clientSocket = accept(socket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        char convertedAddr[32];
        printf("Serve a connection from %s\n", inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, convertedAddr, sizeof(convertedAddr)));
        while (1) {
            //serve connection
            int16_t code;
            char buffer[BUFFER_MAX_SIZE];
            if (ftp_receive(clientSocket, &code, buffer, BUFFER_MAX_SIZE) < 0) {
                printf("Error\n");
                //break and close connect
                break;
            }
            int valid = 0;
            switch (code) {
                case FTP_COMMAND_LIST:
                    valid = serve_command_list(clientSocket);
                    break;
                case FTP_COMMAND_GET:
                    valid = serve_command_get(clientSocket, file_socket, buffer);
                    break;
                case FTP_COMMAND_PUT:
                    valid = serve_command_put(clientSocket, file_socket, buffer);
                    break;
                case FTP_COMMAND_REMOVE:
                    valid = serve_command_remove(clientSocket, buffer);
                    break;
                case FTP_COMMAND_QUIT:
                    printf("Client quit session!\n");
                    valid = -1;
                    break;
                default: 
                    printf("Uknown command from client\n");
                    break;
            }
            if (valid < 0) break; //connection get error
        }
        closeSocket(clientSocket);
    }
    return 0;
}

int serve_command_list(int socket) {
    struct dirent* entry;
    DIR *dp = opendir(ROOT_PATH);
    if (dp == NULL) {
        perror("Root folder missing!\n");
        return -1;
    }
    char buffer[BUFFER_MAX_SIZE];
    clearBuffer(buffer, BUFFER_MAX_SIZE);
    buffer[0] = '\0';
    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            //ignore
        } else {
            printf("%s\n", entry->d_name);
            strcat(buffer, entry->d_name);
            //insert a tab
            strcat(buffer, "    ");
        }
    }
    closedir(dp);
    if (ftp_send(socket, FTP_RESPONSE_SUCCESS, (const void*)buffer, (ssize_t)lengthOfString(buffer, BUFFER_MAX_SIZE)) < 0) {
        printf("Error\n");
        return -1;
    }

    return 0;
}

int serve_command_get(int socket, int fileSocket, char* buffer) {
    if (check_if_exist_file(buffer) < 0) {
        //no exist
        if (ftp_send(socket, FTP_RESPONSE_REJECT, buffer, 0) < 0) return -1;
    } else {

        //open file
        char fileName[200] = "";
        strcpy(fileName, ROOT_PATH);
        strcat(fileName, buffer);
        FILE* fp = fopen(fileName, "rb");

        if (fp == NULL) {
            printf("Can not open file\n");
            if (ftp_send(socket, FTP_RESPONSE_REJECT, buffer, 0) < 0) return -1;
            return 0;
        }

        if (ftp_send(socket, FTP_RESPONSE_WAITFORDOWNLOAD, buffer, lengthOfString(buffer, BUFFER_MAX_SIZE)) < 0) {
            fclose(fp);
            return -1;
        }
        
        //open server
        struct sockaddr_in clientAddr;
        clearBuffer(&clientAddr, sizeof(clientAddr));
        socklen_t clientAddrSize = sizeof(clientAddr);
        int fileTransferSocket = accept(fileSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

        //start sending file
        //time_t now = time(NULL);
        if (ftp_send_file(fileTransferSocket, fp) < 0) {
            printf("Error when sending file to client\n");
            fclose(fp);
            closeSocket(fileTransferSocket);
            return -1;
        }

        //
        printf("Transfer file '%s' successfully to client\n", buffer);
        fclose(fp);
        closeSocket(fileTransferSocket);
    }
    return 0;
}
int serve_command_put(int socket, int fileSocket, char* buffer) {
    char fileName[200] = "";
    strcpy(fileName, ROOT_PATH);
    strcat(fileName, buffer);
    FILE* fp = fopen(fileName, "wb");

    if (fp == NULL) {
        printf("Can not open file\n");
        if (ftp_send(socket, FTP_RESPONSE_REJECT, buffer, 0) < 0) return -1;
        return 0;
    }

    if (ftp_send(socket, FTP_RESPONSE_WAITFORUPLOAD, buffer, lengthOfString(buffer, BUFFER_MAX_SIZE)) < 0) {
        fclose(fp);
        return -1;
    }
    
    //open server
    struct sockaddr_in clientAddr;
    clearBuffer(&clientAddr, sizeof(clientAddr));
    socklen_t clientAddrSize = sizeof(clientAddr);
    int fileTransferSocket = accept(fileSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

    //start sending file
    //time_t now = time(NULL);
    if (ftp_receive_file(fileTransferSocket, fp) < 0) {
        printf("Error when sending file to client\n");
        fclose(fp);
        closeSocket(fileTransferSocket);
        return -1;
    }

    //
    printf("Received file '%s' successfully from client\n", buffer);
    fclose(fp);
    closeSocket(fileTransferSocket);
    return 0;
}
int serve_command_remove(int socket, char* buffer) {
    char fileName[200] = "";
    strcpy(fileName, ROOT_PATH);
    strcat(fileName, buffer);

    if (remove(fileName) == 0) {
        if (ftp_send(socket, FTP_RESPONSE_SUCCESS, buffer, 0) < 0) {
            return -1;
        }
    } else {
        if (ftp_send(socket, FTP_RESPONSE_REJECT, buffer, 0) < 0) {
            return -1;
        }
    }
    return 0;
}


int check_if_exist_file(char* fileName) {
    struct dirent* entry;
    DIR *dp = opendir(ROOT_PATH);
    if (dp == NULL) {
        perror("Root folder missing!\n");
        return -1;
    }
    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            //ignore
        } else {
            if (strcmp(fileName, entry->d_name) == 0) {
                closedir(dp);
                return 0;
            }
        }
    }
    closedir(dp);
    return -1;
}