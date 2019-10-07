/*
    PHAN PHUONG NAM - 17020918
    CACH SU DUNG CLIENT: ./client
*/

/*
    WELCOME TO A MINI UNAUTHENTICATED FTP SERVER
*/

#include "ilib/ilib.h"

const int BUFFER_MAX_SIZE = 1024;
const int FTP_APP_PORT = 3003;
const int FTP_FILE_TRANSFER_PORT = 3004;

//save ipAddr of app
char ipAddr[32];

//main logic
void run(int socket);

//return command code
int checkCommand(char* command, ssize_t lengthOfCommand, void* commandContent, ssize_t maxCommandContentSize);


//command
int command_put(int socket, char* buffer);
int command_get(int socket, char* buffer);
int command_list(int socket);
int command_remove(int socket, char* buffer);
int command_quit(int socket);
int command_ls();
int command_rm(char* buffer);

int main() {
    printf("Welcome to fake FTP client!\n");
    printf("Please insert your FTP IP server to connect: ");
    fgets(ipAddr, sizeof(ipAddr), stdin);
    removeLineBreakCharacter(ipAddr, sizeof(ipAddr));
	int socketConnection = getTCPSocketConnectionIpv4(ipAddr, FTP_APP_PORT);
	if (socketConnection < 0) {
		printf("Connect to server fail\n");
	} else {
		printf("Connect successfully to FTP server!\n");
        run(socketConnection);
	}

    //closeSocket(socketConnection);
	return 0;
}

// main logic
void run(int socket) {
    char buffer[BUFFER_MAX_SIZE];
    clearBuffer(buffer, sizeof(buffer));
    while (1) {
        //read command into buffer
        printf(">>> ");
        fflush(stdin);
        fgets(buffer, BUFFER_MAX_SIZE, stdin);

        //remove "\n character"
        removeLineBreakCharacter(buffer, BUFFER_MAX_SIZE);

        //lowercase all
        //toLowerCase(buffer, BUFFER_MAX_SIZE);

        //printf("Your command: %s\n", buffer);
        
        //check if it's a quit command
        if (strcmp(buffer, "quit") == 0) {
            if (command_quit(socket) < 0) {
                printf("Error\n");
            }
            break;
        }
        int command = checkCommand(buffer, lengthOfString(buffer, BUFFER_MAX_SIZE), (void*)buffer, BUFFER_MAX_SIZE);
        if (command < 0) {
            printf("Your command is invalid\n");
        } else {
            //process the command
            switch (command) {
                case FTP_COMMAND_LIST:
                    if (command_list(socket) < 0) return;
                    break;
                case FTP_COMMAND_GET:
                    if (command_get(socket, buffer) < 0) return;
                    break;
                case FTP_COMMAND_PUT:
                    if (command_put(socket, buffer) < 0) return;
                    break;
                case FTP_COMMAND_REMOVE:
                    if (command_remove(socket, buffer) < 0) return;
                    break;
                case FTP_COMMAND_LS:
                    if (command_ls() < 0) return;
                    break;
                case FTP_COMMAND_RM:
                    if (command_rm(buffer) < 0) return;
                    break;
                default:
                    printf("Unknown command\n");
                    break;
            }
        }
    }
}

int command_list(int socket) {
    char buffer[BUFFER_MAX_SIZE];
    if (ftp_send(socket, FTP_COMMAND_LIST, buffer, 0) < 0) {
        printf("Error\n");
        return -1;
    }
    //receive the file list and render
    int16_t code;
    if (ftp_receive(socket, &code, buffer, BUFFER_MAX_SIZE) < 0) {
        //printf("%d\n", code);
        printf("Error\n");
        return -1;
    }
    if (code == FTP_RESPONSE_REJECT) {
        printf("Can not get the list! Server rejected your request!\n");
    }
    else if (code == FTP_RESPONSE_SUCCESS) {
        //print the file list return from server
        if (lengthOfString(buffer, BUFFER_MAX_SIZE) <= 0) {
            printf("Data folder on server empty\n");
        } else {
            printf("%s\n", buffer);
        }
    } else {
        printf("Unknown response from server!\n");
    }
    return 0;
}

int command_put(int socket, char* buffer) {
    int lengthOfContent = lengthOfString(buffer, BUFFER_MAX_SIZE);
    //try to openfile:
    FILE* fp = fopen(buffer, "rb");
    if (fp == NULL) {
        printf("File is not exist or can not open file\n");
        return 0;
    }
    
    int16_t code;
    if (ftp_send(socket, FTP_COMMAND_PUT, buffer, lengthOfContent) < 0) {
        printf("Error\n");
        return -1;
    }
    if (ftp_receive(socket, &code, buffer, BUFFER_MAX_SIZE) < 0) {
        printf("Error\n");
        return -1;
    }
    if (code == FTP_RESPONSE_WAITFORUPLOAD) {
        //upload
        int file_trans = getTCPSocketConnectionIpv4(ipAddr, FTP_FILE_TRANSFER_PORT);
        if (file_trans < 0) {
            printf("Error when try to trans file\n");
            closeSocket(file_trans);
            fclose(fp);
            return 0;
        }

        struct timespec t_start = {0,0}, t_end = {0,0};
        clock_gettime(_POSIX_MONOTONIC_CLOCK, &t_start);
        printf("Uploading...\n");
        if (ftp_send_file(file_trans, fp) < 0) {
            printf("Error when upload file\n");
            fclose(fp);
            closeSocket(file_trans);
            return 0;
        }
        printf("File upload successfully\n");
        clock_gettime(_POSIX_MONOTONIC_CLOCK, &t_end);
        long time_total = (t_end.tv_sec - t_start.tv_sec) * 1e3 + (t_end.tv_nsec - t_start.tv_nsec) / 1e6;
        long receivedFileSize = ftell(fp);
        printf("Uploaded %ld bytes in %ldms\n", receivedFileSize, time_total);
        if (time_total > 0) {
            printf("Upload speed: %lf bytes/ms\n", ((double)receivedFileSize)/(time_total));
        }
        closeSocket(file_trans);
    } else if (code == FTP_RESPONSE_REJECT) {
        printf("Can not upload this file!\n");
    } else {
        printf("Unknown response from server\n");
    }
    fclose(fp);
    return 0;
}

int command_ls() {
    system("ls");
    return 0;
}

int command_get(int socket, char* buffer) {
    int lengthOfContent = lengthOfString(buffer, BUFFER_MAX_SIZE);
    int16_t code;
    if (ftp_send(socket, FTP_COMMAND_GET, buffer, lengthOfContent) < 0) {
        printf("Error\n");
        return -1;
    }
    if (ftp_receive(socket, &code, buffer, BUFFER_MAX_SIZE) < 0) {
        printf("Error\n");
        return -1;
    }
    if (code == FTP_RESPONSE_WAITFORDOWNLOAD) {
        //printf("Ready for downloading\n");
        FILE* fp = fopen(buffer, "wb");
        if (fp == NULL) {
            printf("Can not create a file on client side\n");
            return 0;
        }
        int file_trans = getTCPSocketConnectionIpv4(ipAddr, FTP_FILE_TRANSFER_PORT);
        if (file_trans < 0) {
            printf("Error when try to trans file\n");
            closeSocket(file_trans);
            fclose(fp);
            return 0;
        }

        //start receive file
        struct timespec t_start = {0,0}, t_end = {0,0};
        clock_gettime(_POSIX_MONOTONIC_CLOCK, &t_start);
        printf("Downloading...\n");
        if (ftp_receive_file(file_trans, fp) < 0) {
            printf("Error when receive file\n");
            fclose(fp);
            closeSocket(file_trans);
            return 0;
        }
        printf("File download successfully\n");
        clock_gettime(_POSIX_MONOTONIC_CLOCK, &t_end);
        long time_total = (t_end.tv_sec - t_start.tv_sec) * 1e3 + (t_end.tv_nsec - t_start.tv_nsec) / 1e6;
        long receivedFileSize = ftell(fp);
        printf("Received %ld bytes in %ldms\n", receivedFileSize, time_total);
        if (time_total > 0) {
            printf("Download speed: %lf bytes/ms\n", ((double)receivedFileSize)/(time_total));
        }
        fclose(fp);
        closeSocket(file_trans);
    } else if (code == FTP_RESPONSE_REJECT) {
        printf("File is not exist or server get error\n");
    } else {
        printf("Unknown response from server\n");
    }
    
    return 0;
}

int command_remove(int socket, char* buffer) {
    if (ftp_send(socket, FTP_COMMAND_REMOVE, buffer, lengthOfString(buffer, BUFFER_MAX_SIZE)) < 0) {
        printf("Error\n");
        return -1;
    }
    int16_t code;
    if (ftp_receive(socket, &code, buffer, BUFFER_MAX_SIZE) < 0) {
        printf("Error\n");
        return -1;
    }
    if (code == FTP_RESPONSE_SUCCESS) {
        printf("Remove file successfully on server\n");
    } else if (code == FTP_RESPONSE_REJECT) {
        printf("Remove file failed. File is not exist or server get error\n");
    } else {
        printf("Unknown response from server\n");
    }
    return 0;
}

int command_rm(char* buffer) {
    system(buffer);
    return 0;
}

int command_quit(int socket) {
    char buffer[1];
    printf("Quit FTP server...\n");
    return ftp_send(socket, FTP_COMMAND_QUIT, buffer, 0);
}

int checkCommand(char* command, ssize_t lengthOfCommand, void* commandContent, ssize_t maxCommandContentSize) {
    int code = -1;
    if (strncmp(command, "list", 4) == 0 || strncmp(command, "LIST", 4) == 0) {
        //this is list command
        code = FTP_COMMAND_LIST;
        if (lengthOfCommand > 4) {
            //list cannot contain any content behind it
            printf("There is too many argument in your command\n");
            return -1;
        }
        clearBuffer(commandContent, maxCommandContentSize);
    }
    else if (strncmp(command, "put ", 4) == 0 || strncmp(command, "PUT ", 4) == 0) {
        //this is put command
        code = FTP_COMMAND_PUT;
        if (lengthOfCommand < 5) {
            printf("Require a argument for this command!\n");
            return -1;
        }
        if (checkSpace(command, 4, lengthOfCommand, commandContent, maxCommandContentSize) < 0) {
            printf("There is too many argument in your command\n");
            return -1;
        }
    } else if (strncmp(command, "get ", 4) == 0 || strncmp(command, "GET ", 4) == 0) {
        //this is get command
        code = FTP_COMMAND_GET;
        if (lengthOfCommand < 5) {
            printf("Require a argument for this command!\n");
            return -1;
        }
        if (checkSpace(command, 4, lengthOfCommand, commandContent, maxCommandContentSize) < 0) {
            printf("There is too many argument in your command\n");
            return -1;
        }

    } else if (strncmp(command, "remove ", 7) == 0 || strncmp(command, "REMOVE ", 7) == 0) {
        //this is remove command
        code = FTP_COMMAND_REMOVE;
        if (lengthOfCommand < 8) {
            printf("Require a argument for this command!\n");
            return -1;
        }
        if (checkSpace(command, 7, lengthOfCommand, commandContent, maxCommandContentSize) < 0) {
            printf("There is too many argument in your command\n");
            return -1;
        }
    } else if (strncmp(command, "ls", 2) == 0 || strncmp(command, "LS", 2) == 0) {
        code = FTP_COMMAND_LS;
        if (lengthOfCommand > 2) {
            //list cannot contain any content behind it
            printf("There is too many argument in your command\n");
            return -1;
        }
        clearBuffer(commandContent, maxCommandContentSize);
    } else if (strncmp(command, "rm ", 3) == 0) {
        code = FTP_COMMAND_RM;
    } 
    return code;
}


