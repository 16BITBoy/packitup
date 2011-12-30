#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include "../piu/piu.h"


int check_newpiustring();
int check_newpiustrarr();
int check_addpiustrarritem();
int check_concatpiustrarr();
int check_createpath();
int check_rmparentpathname();
int check_substr();
int check_piustrsplit();
int check_ispiufile();
int check_getflistsize();
int check_getfileinfolist();
int check_getpiuheader();
int check_openpiufile();
int check_createpiufile();
int check_writepiu();
int check_addfile();
int check_deletefile();
int check_unloadpiufile();

int main(int argc, char **argv){
    check_newpiustring();
    check_newpiustrarr();
    check_addpiustrarritem();
    check_concatpiustrarr();
    check_createpath();
    check_rmparentpathname();
    check_piustrsplit();
    printf("All tests PASSED\n");
    return 0;
}

int check_newpiustring(){
    PIUSTRING *str = NULL;
    str = newpiustring(50);
    assert(str != NULL);
    assert(str->str != NULL);
    assert(*str->str == 0);
    free(str->str);
    free(str);
    return 0;
}

int check_newpiustrarr(){
    PIUSTRARR *strs = NULL;
    strs = newpiustrarr();
    assert(strs != NULL);
    free(strs);
    return 0;
}

int check_addpiustrarritem(){
    PIUSTRING *str1 = newpiustring(50);
    strcpy(str1->str, "texto");
    PIUSTRARR *strs = newpiustrarr();
    addpiustrarritem(strs, str1);
    assert((strcmp(strs->items[0].str, "texto") == 0));
    free(str1->str);
    free(str1);
    free(strs);
    return 0;
}

int check_concatpiustrarr(){
    PIUSTRING *str1 = newpiustring(50);
    strcpy(str1->str, "texto");
    PIUSTRING *str2 = newpiustring(50);
    strcpy(str2->str, "otromas");
    PIUSTRARR *strs = newpiustrarr();
    addpiustrarritem(strs, str1);
    addpiustrarritem(strs, str2);
    PIUSTRING *str3 = concatpiustrarr(strs);
    assert((strcmp(str3->str, "texto/otromas") == 0));
    free(str1->str);
    free(str1);
    free(str2->str);
    free(str2);
    free(str3->str);
    free(str3);
    free(strs);
    return 0;
}

int check_createpath(){
    PIUSTRING *path = newpiustring(50);
    strcpy(path->str, "esto/es/una/prueba");
    PIUSTRARR *strs = piustrsplit(path, '/');
    assert(createpath(path));
    struct stat st;
    int i;
    for(i = 0; i < strs->nitems; i++){
        assert(lstat(strs->items[i].str, &st) != -1);
        chdir(strs->items[i].str);
    }
    for(i = strs->nitems - 1; i >= 0; i--){
        chdir("..");
        if(remove(strs->items[i].str) == -1)
            perror("remove");
    }
    free(path->str);
    free(path);
    free(strs);
    return 0;
}

int check_rmparentpathname(){
    PIUSTRING *str = newpiustring(50);
    strcpy(str->str, "../quitamelos");
    rmparentpathname(str);
    assert(!strcmp(str->str, "quitamelos"));
    strcpy(str->str, "/quitamelo");
    rmparentpathname(str);
    assert(!strcmp(str->str, "quitamelo"));
    free(str->str);
    free(str);
    return 0;
}

int check_piustrsplit(){
    PIUSTRING *str = newpiustring(50);
    PIUSTRARR *strs = NULL;
    strcpy(str->str, "parteme-en-tres");
    strs = piustrsplit(str, '-');
    assert(strs->nitems == 3);
    assert(!strcmp(strs->items[0].str, "parteme"));
    assert(!strcmp(strs->items[1].str, "en"));
    assert(!strcmp(strs->items[2].str, "tres"));
    strcpy(str->str, "parteme/en/cua/tro");
    strs = piustrsplit(str, '/');
    assert(strs->nitems == 4);
    assert(!strcmp(strs->items[0].str, "parteme"));
    assert(!strcmp(strs->items[1].str, "en"));
    assert(!strcmp(strs->items[2].str, "cua"));
    assert(!strcmp(strs->items[3].str, "tro"));
    strcpy(str->str, "no me partas");
    strs = piustrsplit(str, '&');
    assert(strs->nitems == 1);
    assert(!strcmp(strs->items[0].str, "no me partas"));
    free(str->str);
    free(str);
    free(strs);
    return 0;
}

   
