#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piu/piu.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>


int deletefile(PIUFILE *piu, unsigned long filenum){
#define FCOUNT piu->header->filelist.filecount
#define FINFO piu->header->filelist.fileinfo

    if(FCOUNT == 0)return 0;

    unsigned long i;
    for(i = filenum; i < FCOUNT - 1; i++){
        FINFO[i] = FINFO[i+1];    
        piu->filedata[i] = piu->filedata[i+1];
    }
    FINFO =
    (FILEINFO *) realloc(FINFO,
                         sizeof(FILEINFO) * (FCOUNT - 1));
    piu->filedata = (DATA *) realloc(piu->filedata, 
                                         sizeof(DATA) * (FCOUNT -1));
    piu->header->flistsize = piu->header->flistsize - FILEINFO_SIZE;
    FCOUNT = FCOUNT - 1;
    return 1;
}


int main(int argc, char **argv){
    PIUFILE *piu = openpiufile(argv[1]);
    unsigned long filenum = atol(argv[2]);
    if(piu == NULL){
        printf("No se ha podido abrir el archivo \'%s\'\n", argv[1]);
        exit(1);
    }
    if(!deletefile(piu, filenum)){
        printf("Ha ocurrido un fallo al intentar borrar el archivo.\n");
        exit(2);
    }
    if(!writepiu(piu, argv[1])){
        perror("writepiu");
        printf("Ocurrió un fallo mientras se escribia el fichero a disco.\n");
        exit(3);
    }
    printf("El fichero se ha eliminado exitosamente!\n");
    return 0;
}
