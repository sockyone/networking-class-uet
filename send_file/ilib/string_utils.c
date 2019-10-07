#ifndef STRING_UTILS
#define STRING_UTILS

#include "ilib.h"

void removeLineBreakCharacter(char* string, ssize_t maxSize) {
    //remove last \n character
    int i = 0;
    for (i = 0; i < maxSize; i++) {
        if (*(string+i) == '\n') {
            *(string+i) = '\0';
            return;
        }
    }
}


void toLowerCase(char* string, ssize_t maxSize) {
    int i = 0;
    for (i = 0; i < maxSize; i++) {
        if (*(string+i) == '\0') {
            //meet the end of string -> stop processing
            return;
        }
        if ((int)(*(string+i)) > 64 && (int)(*(string+i)) < 91) {
            //if this is a uppercase -> convert it into lowercase
            *(string+i) = (char)((int)*(string+i) + 32); 
        }
    }
}

int lengthOfString(char* string, ssize_t maxSize) {
    int i = 0;
    for (i = 0; i < maxSize; i++) {
        if (*(string+i) == '\0') return i;
    }
    return 0;
}

int checkSpace(char* string, int start, int end, char* buffer, ssize_t maxBufferSize) {
    int i = start;
    if ((end - start) > maxBufferSize) {
        //invalid copy
        return -1;
    }
    for (i = start; i < end; i++) {
        if (*(string + i) == ' ') {
            //invalid
            return -1;
        }
    }
    strncpy(buffer, string + start, end-start+1);
    return 0;
}

#endif