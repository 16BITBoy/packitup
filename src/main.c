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

PIUSTRARR *piustrsplit2(PIUSTRING *str, char sep){
    PIUSTRARR *strs = NULL;
    int splits = 0;
    int chksize = 0;
    int lastmatch = 0;
    int i;
    /*strs = (PIUSTRARR *)malloc(sizeof(PIUSTRARR));*/
    strs = newpiustrarr();
    for(i = 0; i <= str->len; i++){ /* <= str->len may bring errors */
        if(str->str[i] == sep){

            strs->items = (PIUSTRING *)realloc(strs->items, sizeof(PIUSTRING) * (splits + 1));
            strs->nitems = splits + 1;
            strs->items[splits].str = NULL;
            strs->items[splits].str = (char *)malloc(sizeof(char) * chksize);
            memset(strs->items[splits].str, 0, chksize);
            strs->items[splits].len = chksize;

            substr(strs->items[splits].str, str->str, lastmatch, i);

            splits += 1;
            chksize = 0;
            lastmatch = i + 1;
            if(str->str[i] == '\0')break;
        }
        else{
            chksize += 1;
        }
    }
    strs->items = (PIUSTRING *)realloc(strs->items, sizeof(PIUSTRING) * (splits + 1));
    strs->nitems = splits + 1;
    strs->items[splits].str = (char *)malloc(sizeof(char) * chksize);
    strs->items[splits].len = chksize;
    substr(strs->items[splits].str, str->str, lastmatch, i);

    return strs;
}
int main(int argc, char **argv){
    PIUSTRING *path = newpiustring(strlen("codigo/prueba/Quick Currency Converter"));
    strcpy(path->str, "codigo/prueba/Quick Currency Converter");
    PIUSTRARR *dirs = piustrsplit2(path, '/');

    int i;
    for(i = 0; i < dirs->nitems; i++){
        printf("%s\n",dirs->items[i].str);
    }
    
    return 0;
}
