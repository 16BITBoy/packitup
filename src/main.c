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
    if(fd < 0){
        set_errorno(E_CANNOTOPENFILE);
        return 0;
    }
    
    if(savetofile(filepath, writebuf) != 3){
        set_errorno(E_CANNOTWRITEFILE);
        return 0;
    }
    
    int hsize = FILEINFO_SIZE * piu->header->filelist.filecount;
    writebuf->data = &hsize;
    writebuf->size = sizeof(int);
    
    if(appendtofd(fd, writebuf) != sizeof(int)){
        set_errorno(E_CANNOTWRITEFILE);
        return 0;
    }
    
     /* Better create some defines for short naming */
    #define FINFO    piu->header->filelist.fileinfo
    #define FCOUNT   piu->header->filelist.filecount
    int i;
    for(i = 0; i < FCOUNT; i++){
        writebuf->data = FINFO[i].filename;
        writebuf->size = 256; 
        if(appendtofd(fd, writebuf) != 256){
            set_errorno(E_CANNOTWRITEFILE);
            return 0;
        }
    }

    /* TODO: Write file data */

    freedata(writebuf);
    close(fd);
    return 1;
}

int main(int argc, char **argv){
    PIUFILE *piu;
    piu = (PIUFILE *) malloc(sizeof(PIUFILE));
    piu->header = (HEADERINFO *) malloc(sizeof(HEADERINFO));
    piu->header->filelist.fileinfo = (FILEINFO *) malloc(sizeof(FILEINFO) * 2);
    piu->header->filelist.fileinfo[0].filename = (char *) malloc(sizeof(char)*256);
    piu->header->filelist.fileinfo[1].filename = (char *) malloc(sizeof(char)*256);
    piu->header->filelist.fileinfo[0].size = 5;
    strcpy(piu->header->filelist.fileinfo[0].filename, "Perez");
    piu->header->filelist.fileinfo[1].size = 6;
    strcpy(piu->header->filelist.fileinfo[1].filename, "Adrian");
    piu->header->filelist.filecount = 2;

    if(!writepiu(piu, "piufile")){
        printf("\nEl fichero no se ha podido escribir correctamente.\n");
        return 1;
    }
    printf("\nEl fichero se ha escrito con exito\n");
    return 0; 
}
