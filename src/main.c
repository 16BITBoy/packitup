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
    DIR *dirp;
    struct stat statbuf;
    struct dirent *entry;
    
    if((dirp = opendir(path)) == NULL){
        fprintf(stderr, "cannot open directory: %s\n", path);
        return 1;
    }

    chdir(path);

    while((entry = readdir(dirp)) != NULL){
        lstat(entry->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode)){
            
    }
}
