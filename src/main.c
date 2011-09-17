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

/* TODO: Most ugliest code I've ever writen. Clean and refactor needed */
int listallfiles(char *path, PIUSTRARR *filelist, PIUSTRARR *buf){
    DIR *dirp;
    struct stat statbuf;
    struct dirent *entry;
    PIUSTRING *fname = NULL;
    PIUSTRING *cwd = NULL;
    
    if((dirp = opendir(path)) == NULL){
        fprintf(stderr, "cannot open directory: %s\n", path);
        return 0;
    }

    chdir(path);
    
    /* If we are in the first directory, add just
     * an empty piustring to the path buffer 'buf'.
     * I've writen this part about 1 or 2 hours ago
     * now I don't remember why I had to do this u.u */
    if(strcmp(path, ".") == 0)
        addpiustrarritem(buf, newpiustring(1));
    else
    {   /* Add the current dir to the path buffer 'buf' */
        PIUSTRING *str = newpiustring(256);
        strcpy(str->str, path);
        addpiustrarritem(buf, str);
    }

    while((entry = readdir(dirp)) != NULL){
        if(lstat(entry->d_name, &statbuf) == -1){
            perror("stat");
            chdir("..");
            return 0;
        }
        if(S_ISDIR(statbuf.st_mode)){
            if(strcmp(entry->d_name, ".") != 0 &&
               strcmp(entry->d_name, "..") != 0)
            {
                listallfiles(entry->d_name, filelist, buf);
            }    
        }
        if(S_ISREG(statbuf.st_mode)){
            fname = concatpiustrarr(buf);
            /* Delete the first '/' symbol
             * because adding that extra empty entry
             * in the path buffer. */
            int i;
            for(i = 0; i < fname->len - 1; i++){
                fname->str[i] = fname->str[i+1];
            }

            if(buf->nitems > 1)strcat(fname->str, "/");
            strcat(fname->str, entry->d_name);
            addpiustrarritem(filelist, fname);
        }
    }
    chdir("..");
    buf->nitems--;
    return 1;
}

int main(int argc, char **argv){
    char *path = argv[1];
    PIUSTRARR *filelist = newpiustrarr();
    PIUSTRARR *pathbuf =newpiustrarr();
    listallfiles(path, filelist, pathbuf);

    int i;
    for(i = 0; i < filelist->nitems; i++){
        printf("\n%s\n", filelist->items[i].str);
    }

    return 0;
}
