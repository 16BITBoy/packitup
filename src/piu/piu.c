#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piu.h"

/*FIXME: There is a ugly thing here. Read one more byte to avoid reallocating */
int is_piu_file(int fd){

    DATA *dt = load_chk_file(fd, 0, 4);

    if(dt == NULL){
        set_errorno(E_CANNOTREADFILE);
        return 0;
    }
        
    char piu[4];
    strcpy(piu, "PIU");
    
    char *input = (char *) dt->data;
    input[3] = '\0';
    
    if(strcmp(piu, input) != 0){
        
        set_errorno(E_PIUFILENOTVALID);
        return 0; // not PIU file
    }
        
    return 1;
}

int get_flistsize(int fd){
    DATA *buffer;

    buffer = load_chk_file(fd, 3, 4);
    if(buffer == NULL){
        set_errorno(E_PIUFILENOTVALID);
        return -1;
    }
    
    int *flistsize;
    flistsize = (int *) buffer->data;

    return *flistsize;
}

FILEINFO *get_fileinfolist(int fd, int numfiles){
    FILEINFO *finfolist;
    finfolist = (FILEINFO *) malloc(sizeof(FILEINFO) * numfiles);
    
    DATA *buffer;
    unsigned long *ulptr;

    unsigned long pos = 7;

    int i;
    for(i = 0; i < numfiles; i++){
        buffer = load_chk_file(fd, pos, 256);
        if(buffer == NULL){
            set_errorno(E_CANNOTREADFILE);
            return NULL;
        }
        
        finfolist[i].filename = (char *) buffer->data;

        pos = pos + 256;

        buffer = load_chk_file(fd, pos, sizeof(unsigned long));
        if(buffer == NULL){
            set_errorno(E_CANNOTREADFILE);
            return NULL;
        }

        ulptr = (unsigned long *) buffer->data;
        finfolist[i].size = *ulptr; 

        pos = pos + sizeof(unsigned long);
    }

    return finfolist;
}

HEADERINFO *get_piu_header(int fd){
    HEADERINFO *header;

    if(fd < 0){ //Check for valid file descriptor
        set_errorno(E_CANNOTREADFILE);
        return NULL;
    }

    if(!is_piu_file(fd))
        return NULL;

    header = (HEADERINFO *) malloc(sizeof(HEADERINFO)); 
    
    header->flistsize = get_flistsize(fd); //Retrieves filelist size
    if(header->flistsize < 0)
        return NULL;
        
    header->filelist.filecount = header->flistsize / FILEINFO_SIZE; //How many files do we have?

    header->filelist.fileinfo = get_fileinfolist(fd, header->filelist.filecount); //Retrieving file information list
    if(header->filelist.fileinfo == NULL)
        return NULL;

    return header;
}

PIUFILE *open_piu_file(char *file){
    PIUFILE *piu;
    piu = (PIUFILE *) malloc(sizeof(PIUFILE));

    int fd = file_open(file, FILE_MUST_EXIST);
    piu->header = get_piu_header(fd);
    if(piu->header == NULL)
        return NULL;
    close(fd);
    return piu;    
}

void unload_piu_file(PIUFILE *piu){
    /*Unload file data*/
    /*Unload header section*/
}
