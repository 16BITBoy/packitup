#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "../piu/piu.h"

int listallfiles(char *path, PIUSTRARR *filelist){
    DIR *dirp;
    struct stat statbuf;
    struct dirent *entry;
    PIUSTRING *fname = NULL;
    PIUSTRING *cwd = NULL;

    char curitempath[PATH_MAX];
    if((dirp = opendir(path)) == NULL){
        fprintf(stderr, "cannot open directory: %s\n", path);
        return 0;
    }
    
    while((entry = readdir(dirp)) != NULL){
        strcpy(curitempath, path);
        strcat(curitempath, "/");
        strcat(curitempath, entry->d_name);
        if(lstat(curitempath, &statbuf) == -1){
            char errtitle[PATH_MAX];
            strcpy(errtitle, "Error stating ");
            strcat(errtitle, entry->d_name);
            perror(errtitle);
            printf("\nCurrent path is: %s\n", path);
            return 0;
        }
        if(S_ISDIR(statbuf.st_mode)){
            if(strcmp(entry->d_name, ".") != 0 &&
               strcmp(entry->d_name, "..") != 0)
            {
                listallfiles(curitempath, filelist);
            }    
        }
        if(S_ISREG(statbuf.st_mode)){
            fname = newpiustring(PATH_MAX);
            strcpy(fname->str, curitempath);
            addpiustrarritem(filelist, fname);
        }
    }
    return 1;
}



int main(int argc, char **argv){
    if(strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0){
        printf("piuadd - Pack It Up packager\nver:0.1.8\n\n");
        return 0;
    }
    
    PIUSTRARR *filelist = newpiustrarr();
    if(argc < 3){
        printf("Usage: piuadd PIUFILE FILE...\n\n");
        exit(1);
    }

    /* Load PIU file and the file to be added */
    PIUFILE *piu = openpiufile(argv[1]);
    if(piu == NULL){
        piu = createpiufile();
    }
    
    /* Get all files from directories if there is one as argument */
    int i;
    for(i = 2; i < argc; i++){
        struct stat statbuf;
        lstat(argv[i], &statbuf);
        if(S_ISDIR(statbuf.st_mode)){
            listallfiles(argv[i], filelist);
        }
        if(S_ISREG(statbuf.st_mode)){
            PIUSTRING *fname = newpiustring(PATH_MAX);
            strcpy(fname->str, argv[i]);
            addpiustrarritem(filelist, fname);
        }
            
    }

    /* Adding files */
    
    float prog = 0;
    printf("\nAdding new files...\n");
    for(i = 0; i < filelist->nitems; i++){
        prog = (100 / filelist->nitems) * i;
        printf("\n%f ...\n", prog);
        if(!addfile(piu, filelist->items[i].str)){
            set_errorno(E_CANNOTREADFILE);
            printf("\n%s\n",piu_errmsg(piu_errno));
            exit(2);
        }
    }
    printf("\n100\% ...\n"); 
    printf("\n\nWriting file...\n\n");
    if(!writepiu(piu, argv[1])){
        set_errorno(E_CANNOTWRITEFILE);
        printf("\n%s\n",piu_errmsg(piu_errno));
        exit(3);
    }
    
    return 0;
}
