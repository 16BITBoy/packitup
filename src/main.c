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
    filedata = load_file(filepath);
    if(filedata == NULL){
        set_errorno(E_CANNOTREADFILE);
        return 0;
    }
    
    /* Add fileinfo to header */
    piu->header->filelist.fileinfo = 
    (FILEINFO *) realloc(piu->header->filelist.fileinfo, /* FIXME: If this sencence fails, we wont know it*/
                         sizeof(FILEINFO) * (piu->header->filelist.filecount + 1)); 
    
    /*  Storing filename into the data structure. We take the name from the filepath parameter.
     * We need to allocate 256 bytes for store the filename.
     */
    piu->header->filelist.fileinfo[piu->header->filelist.filecount].filename = (char *) malloc(256);
    strcpy(piu->header->filelist.fileinfo[piu->header->filelist.filecount].filename, filepath);
    
    piu->header->filelist.fileinfo[piu->header->filelist.filecount].size = filedata->size;

    /* Adding the new file data to the structure */ 
    piu->filedata = (DATA *) realloc(piu->filedata, /* FIXME: If this sentence fails, we wont know it*/ 
                                     sizeof(DATA) * (piu->header->filelist.filecount + 1));
    piu->filedata[piu->header->filelist.filecount] = *filedata;



    return 1;
}



int main(int argc, char **argv){
    PIUFILE *piu = open_piu_file(argv[1]);
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
