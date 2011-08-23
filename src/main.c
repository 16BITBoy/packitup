#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piu/piu.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
/* LIST OF TESTS FUNCTIONS 
 *
 * Only for testing purposes, this is not part of PackItUp
 *
 * */

typedef struct{
    char *str;
    int len;
}PIUSTRING;

typedef struct{
    PIUSTRING *items;
    int nitems;
}PIUSTRARR;

int substr(char *dest, char *src, int start, int end){
    int j = 0;
    int i = 0;
    for(i = start; i < end; i++){
        dest[j] = src[i];
        j++;
    }
    return j;
}

PIUSTRARR *piustrsplit(PIUSTRING *str, char sep){
    PIUSTRARR *strs = NULL;
    int splits = 0;
    int chksize = 0;
    int lastmatch = 0;
    int i;
    strs = (PIUSTRARR *)malloc(sizeof(PIUSTRARR));
    for(i = 0; i <= str->len; i++){ /* <= str->len may bring errors */
        if(str->str[i] == sep){
            strs->items = (PIUSTRING *)realloc(strs->items, sizeof(PIUSTRING) * (splits + 1));
            strs->nitems = splits + 1;
            strs->items[splits].str = (char *)malloc(sizeof(char) * chksize);
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

PIUSTRING *rmparentpathname(PIUSTRING *path){
    int i;
    if(path->str[0] == '.' && path->str[1] == '.' 
                           && path->str[2] == '/')
    {
        for(i = 3; i < path->len; i++){
            path->str[i-3] = path->str[i];
        }
        path->len -= 3;
    }else if(path->str[0] == '/'){
        for(i = 1; i < path->len; i++){
            path->str[i-1] = path->str[i];
        }
        path->len -= 1;
    }
    return path; 
}

int createpath(PIUSTRING *path){
    PIUSTRARR *strs = (PIUSTRARR *)malloc(sizeof(PIUSTRARR));
    path = rmparentpathname(path);
    strs = piustrsplit(path, '/');
    int i;
    for(i = 0; i < strs->nitems; i++){
        if(mkdir(strs->items[i].str, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP) != 0){
            return 0;
        }
        if(chdir(strs->items[i].str) != 0){
            return 0;
        }
    }
    for(;i > 0; i--){
        if(chdir("..") != 0){
            return 0;
        }
    }
    return 1;
}


PIUSTRING *newpiustring(int len){
    PIUSTRING *str = (PIUSTRING *)malloc(sizeof(PIUSTRING));
    str->str = (char *)malloc(sizeof(char) * len);
    str->len = len;
    return str;
}

PIUSTRARR *newpiustrarr(){
    PIUSTRARR *strs = (PIUSTRARR *)malloc(sizeof(PIUSTRARR));
    return strs;
}

PIUSTRARR *addpiustrarritem(PIUSTRARR *strs, PIUSTRING *str){
    strs->items = (PIUSTRING *)
                  realloc(strs->items, 
                          sizeof(PIUSTRING) * (strs->nitems + 1));
    strs->items[strs->nitems].str = str->str;
    strs->items[strs->nitems].len = str->len;
    strs->nitems++;
    return strs;
}

int main(int argc, char **argv){
    
    PIUSTRING *str = newpiustring(256);
    strcpy(str->str, "/unacarpeta/otracarpeta/yotramas");
    if(createpath(str)){
        printf("\nLa ruta se ha creado con exito!\n");
    }else{
        printf("\nNo se ha podido crear la ruta completa\n");
    }
    /*
    str = rmparentpathname(str);
    printf("\npath: %s\n", str->str);
    strs = piustrsplit(str, '/');
    int i;
    for(i = 0; i < strs->nitems; i++){
        printf("\n%s\n", strs->items[i].str);
    }
    */
    return 0;
}
