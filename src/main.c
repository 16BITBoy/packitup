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
    char *path = argv[1];
    PIUSTRARR *filelist = newpiustrarr();
    listallfiles2(path, filelist);

    int i;
    for(i = 0; i < filelist->nitems; i++){
        printf("\n%s\n", filelist->items[i].str);
    }

    return 0;
}
