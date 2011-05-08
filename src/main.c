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
    DATA *filedata;
    filedata = load_file(filepath);
    if(filedata == NULL){
        set_errorno(E_CANNOTREADFILE);
        return 0;
    }
    
    /* Add fileinfo to header */

    piu->header->filelist.fileinfo = 
    (FILEINFO *) realloc(piu->header->filelist.fileinfo, 
                         sizeof(FILEINFO) * (piu->header->filelist.filecount + 1)); 
    
    strcpy(piu->header->filelist.fileinfo[piu->header->filelist.filecount].filename, filepath);
    
    piu->header->filelist.fileinfo[piu->header->filelist.filecount].size = filedata->size;

    /*FIXME: Try to find a way to check if this sentence above fails*/
    piu->filedata = (DATA *) realloc(piu->filedata, 
                                     sizeof(DATA) * (piu->header->filelist.filecount + 1));
    piu->filedata[piu->header->filelist.filecount] = *filedata;

    return 1;

}

int main(int argc, char **argv){
    PIUFILE *piu = open_piu_file(argv[1]);
    if(piu == NULL){
        printf("%s\n", piu_errmsg(piu_errno));
        return 1;
    }
    printf("\nEl fichero se ha cargado con exito\n");
    return 0; 
}
