#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fdhandler/fdhandler.h"
#include "errors/errors.h"
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

    piu->header.filelist.fileinfo = 
    (FILEINFO *) realloc(piu->header.filelist.fileinfo, 
                         sizeof(FILEINFO) * (piu->header.filelist.filecount + 1)); 
    
    char filename[256]; /* This is null terminated */
    strcat(filename, "\0");
    strncpy(piu->header.filelist.fileinfo[piu->header.filelist.filecount].filename, filename);
    
    piu->header.filelist.fileinfo[piu->header.filelist.filecount].size = filedata->size;

    /*FIXME: Try to find a way to check if this sentence above fails*/
    piu->filedata = (DATA *) realloc(piu->filedata, 
                                     sizeof(DATA) * (piu->header.filelist.filecount + 1));
    piu->filedata[piu->header.filelist.filecount] = filedata->data;

    return 1;

}

int main(int argc, char **argv){
    
    return 0; 
}
