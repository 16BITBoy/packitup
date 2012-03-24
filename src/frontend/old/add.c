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
    static unsigned long nf = 0;
    PIUSTRING *fname = NULL;
    PIUSTRING *cwd = NULL;
    PIUSTRING *dname = NULL;
    PIUSTRARR *dirlist = newpiustrarr();
    char curitempath[PATH_MAX];
    memset(curitempath,0, PATH_MAX);
    if((dirp = opendir(path)) == NULL){
        fprintf(stderr, "cannot open directory: %s\n", path);
        perror("opendir");
        printf("numero de ficheros abiertos por piuadd: %ld \n", nf);
        return 0;
    }
    nf++;
    while((entry = readdir(dirp)) != NULL){
        strcpy(curitempath, path);
        if(path[strlen(path)-1] != '/')strcat(curitempath, "/");
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
                /*listallfiles(curitempath, filelist);*/
                dname = newpiustring(PATH_MAX);
                strcpy(dname->str, curitempath);
                addpiustrarritem(dirlist, dname);
            }    
        }
        if(S_ISREG(statbuf.st_mode)){
            fname = newpiustring(PATH_MAX);
            strcpy(fname->str, curitempath);
            addpiustrarritem(filelist, fname);
        }
    }
    closedir(dirp);

    unsigned long diri = 0;
    for(diri = 0; diri < dirlist->nitems; diri++){
        listallfiles(dirlist->items[diri].str, filelist);
    }

    return 1;
}

void showhelp(const char *msg){
    if(msg != NULL)printf("Error : %s\n", msg);
    printf("Usage : piuadd PIUFILE FILE...\n\n");
}

int main(int argc, char **argv){
    if(argc < 2){
       showhelp("Numero de argumentos incorrecto...");
       exit(1);
    }
    if(strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0){
        printf("piuadd - Pack It Up packager\nver:0.1.8\n\n");
        return 0;
    }
    if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
        showhelp(NULL);
        exit(0);
    }
    if(argc < 3){
        showhelp("Se necesita un parametro mas.");
        exit(1);
    }

    PIUSTRARR *filelist = newpiustrarr();
    /* Load PIU file and the file to be added */
    PIUFILE *piu = openpiufile(argv[1]);
    if(piu == NULL){
        piu = createpiufile();
    }
    /* Get all files from directories if there is one as argument */
    unsigned long i;
    for(i = 2; i < argc; i++){
        struct stat statbuf;
        if(lstat(argv[i], &statbuf) == -1){
            char msg[256];
            snprintf(msg, 256, "Error opening <<%s>> ", argv[i]);
            perror(msg);
            exit(1);
        }
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
    double prog = 0;
    double nfiles = filelist->nitems;
    printf("\nAdding new files...\n");
    for(i = 0; i < filelist->nitems; i++){
        prog = (100 / nfiles) * i;
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
