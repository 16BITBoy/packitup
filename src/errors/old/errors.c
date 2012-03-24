#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"


int piu_errno = 0;

void set_errorno(int number){
    piu_errno = number;
}

char *piu_errmsg(int errno){
    char *msg;
    char errnostr[4];
    
    msg = (char *) malloc(sizeof(char) * 256);

    strcpy(msg, "Error: ");

    switch(errno)
    {
        case E_CANNOTREADFILE:
            strcat(msg, "Cannot read from file. ");
            break;
        case E_CANNOTWRITEFILE:
            strcat(msg, "Cannot write to file. ");
            break;
        case E_PIUFILENOTVALID:
            strcat(msg, "The file seems to have an invalid format, or is not a PIU file. ");
            break;
        default:
            strcat(msg, "An unexpected error happened. ");
    }
    
    strcat(msg, "( Errno: ");
    sprintf(errnostr, "%d", errno);
    strcat(msg, errnostr);
    strcat(msg, " ).");
    
    return msg;
}
            
