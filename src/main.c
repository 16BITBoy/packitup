#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piu/piu.h"

/* LIST OF TESTS FUNCTIONS 
 *
 * Only for testing purposes, this is not part of PackItUp
 *
 * */


int add_file(PIUFILE *piu, char *filepath){
    /* Load file and check if it's readable */
    DATA *filedata;
    filedata = loadfile(filepath);
    if(filedata == NULL){
        set_errorno(E_CANNOTREADFILE);
        return 0;
    }
    
    /* Better create some defines for short naming */
    #define FINFO    piu->header->filelist.fileinfo
    #define FCOUNT   piu->header->filelist.filecount
    
    /* Add fileinfo to header */
    /* FIXME: If this sentence fails, we wont know it */
    FINFO = 
    (FILEINFO *) realloc(FINFO, 
                         sizeof(FILEINFO) * (FCOUNT + 1)); 
    
    /* Storing filename into the data structure. We take the name from
     * the filepath parameter.
     * We need to allocate 256 bytes for store the filename.
     */
    FINFO[FCOUNT].filename = (char *) malloc(256);
    strcpy(FINFO[FCOUNT].filename, filepath);
    
    FINFO[FCOUNT].size = filedata->size;

    /* Adding the new file data to the structure */
    /* FIXME: If this sentence fails, we wont know it */ 
    piu->filedata = (DATA *) realloc(piu->filedata,  
                                     sizeof(DATA) * (FCOUNT + 1));
    piu->filedata[FCOUNT] = *filedata;

    return 1;
}

int writepiu(PIUFILE *piu, char *filepath){
    DATA *writebuf;
    writebuf = (DATA *) malloc(sizeof(DATA));
    
    char format[3] = "PIU";
    writebuf->data = format;
    writebuf->size = 3;
    
    int fd = fileopen(filepath, FCREATE);
    if(savetofile(filepath, writebuf) != 3){
        set_errorno(E_CANNOTWRITEFILE);
        return 0;
    }
    
    int hsize = FILEINFO_SIZE * piu->header->filelist.filecount;
    writebuf->data = &hsize;
    writebuf->size = sizeof(int);
    
    if(appendtofd(fd, hsize) != sizeof(int)){
        set_errorno(E_CANNOTWRITEFILE);
        return 0;
    }

    
    
    return 0;
}

int main(int argc, char **argv){
    PIUFILE *piu = openpiufile(argv[1]);
    if(piu == NULL){
        printf("\nFallo al abrir el fichero:\n");
        printf("%s\n", piu_errmsg(piu_errno));
        return 1;
    }

    if(!add_file(piu, argv[2])){
        printf("\nFallo al añadir un nuevo fichero:\n");
        printf("%s\n", piu_errmsg(piu_errno));
        return 1;
    }

    printf("\nEl fichero se ha cargado con exito\n");
    return 0; 
}
