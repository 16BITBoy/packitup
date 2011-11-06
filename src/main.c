#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piu/piu.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

/* LIST OF TESTS FUNCTIONS 
 *
 * Only for testing purposes, this is not part of PackItUp
 *
 * */

int main(int argc, char **argv){
    char *phrase = "Esto es una frase";
    char *ss = (char *) malloc(sizeof(char) * 4);
    ss[0] = phrase[1];
    printf("%c\n", phrase[1]);
    printf("%s\n", ss);
    printf("%p\n", ss);
    printf("%p\n", phrase+1);
    
    return 0;
}
