#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "piu.h"

#define FCOUNT  piu->header->filelist.filecount
#define FINFO   piu->header->filelist.fileinfo

PIUSTRING *newpiustring(int len){
    PIUSTRING *str = (PIUSTRING *)malloc(sizeof(PIUSTRING));
    str->str = (char *)malloc(sizeof(char) * len);
    memset(str->str, 0, len);
    str->len = len;
    return str;
}

PIUSTRARR *newpiustrarr(){
    PIUSTRARR *strs = (PIUSTRARR *)malloc(sizeof(PIUSTRARR));
    strs->items = NULL;
    strs->nitems = 0;
    return strs;
}

PIUSTRARR *addpiustrarritem(PIUSTRARR *strs, PIUSTRING *str){
    strs->items = (PIUSTRING *)
                  realloc(strs->items, 
                          sizeof(PIUSTRING) * (strs->nitems + 1));
    strs->items[strs->nitems].str = str->str;
    strs->items[strs->nitems].len = str->len;
    strs->nitems++;
    return strs;
}

PIUSTRING *concatpiustrarr(PIUSTRARR *strs){
    PIUSTRING *str = NULL;
    int finallen = 0;
    int i;
    for(i = 0; i < strs->nitems; i++){
        finallen += strs->items[i].len + 1; /* Giving one more byte to the count
                                               since we need to put a slash */
    }
    finallen--; /* We don't need a last slash */
    str = newpiustring(finallen);
    for(i = 0; i < strs->nitems; i++){
        strcat(str->str, strs->items[i].str);
        if(i < strs->nitems - 1)
            strcat(str->str, "/");
    }
    return str;
}
int createpath(PIUSTRING *path){
    PIUSTRARR *strs = (PIUSTRARR *)malloc(sizeof(PIUSTRARR));
    path = rmparentpathname(path);
    strs = piustrsplit(path, '/');
    struct stat st;
    int i;
    for(i = 0; i < strs->nitems; i++){
        if(lstat(strs->items[i].str, &st) == -1 && errno == ENOENT){
            if(mkdir(strs->items[i].str, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP) != 0){
                perror("error creando directorio");
                printf("Directorio: %s\n", strs->items[i].str);
                return 0;
            }
        }
        if(chdir(strs->items[i].str) != 0){
            return 0;
        }
    }
    for(;i > 0; i--){
        if(chdir("..") != 0){
            return 0;
        }
    }
    return 1;
}

PIUSTRING *rmparentpathname(PIUSTRING *path){
    int clean = 0;
    int i;
    
    while(!clean){
        if(path->str[0] == '.' && path->str[1] == '.' 
                               && path->str[2] == '/')
        {
            for(i = 3; i < path->len; i++){
                path->str[i-3] = path->str[i];
            }
            path->len -= 3;
        }else if(path->str[0] == '/'){
            for(i = 1; i < path->len; i++){
                path->str[i-1] = path->str[i];
            }
            path->len -= 1;
        }else{
            clean = 1;
        }
    }
    return path; 
}
int substr(char *dest, char *src, int start, int end){
    int j = 0;
    int i = 0;
    for(i = start; i < end; i++){
        dest[j] = src[i];
        ++j;
    }
    dest[end+1] = '\0';
    return j;
}

PIUSTRARR *piustrsplit(PIUSTRING *str, char sep){
    PIUSTRARR *strs = NULL;
    int splits = 0;
    int chksize = 0;
    int lastmatch = 0;
    int i;
    /*strs = (PIUSTRARR *)malloc(sizeof(PIUSTRARR));*/
    strs = newpiustrarr();
    for(i = 0; i <= str->len; i++){ /* <= str->len may bring errors */
        if(str->str[i] == sep){
            /*this is just for debug fixpiuextract1*/
            strs->items = (PIUSTRING *)realloc(strs->items, sizeof(PIUSTRING) * (splits + 1));
            strs->nitems = splits + 1;
            strs->items[splits].str = NULL;
            strs->items[splits].str = (char *)malloc(sizeof(char) * (chksize + 1));
            memset(strs->items[splits].str, 0, chksize);
            strs->items[splits].len = chksize;

            substr(strs->items[splits].str, str->str, lastmatch, i);

            splits += 1;
            chksize = 0;
            lastmatch = i + 1;
            if(str->str[i] == '\0')break;
        }
        else{
            chksize += 1;
        }
    }
    strs->items = (PIUSTRING *)realloc(strs->items, sizeof(PIUSTRING) * (splits + 1));
    strs->nitems = splits + 1;
    strs->items[splits].str = (char *)malloc(sizeof(char) * chksize);
    strs->items[splits].len = chksize;
    substr(strs->items[splits].str, str->str, lastmatch, i);

    return strs;
}
/*FIXME: There is a ugly thing here. Read one more byte to avoid reallocating */
int ispiufile(int fd){
    DATA *dt = loadchkfile(fd, 0, 4);
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

int getflistsize(int fd){
    DATA *buffer;
    buffer = loadchkfile(fd, 3, 4);
    if(buffer == NULL){
        set_errorno(E_PIUFILENOTVALID);
        return -1;
    }
    
    int *flistsize;
    flistsize = (int *) buffer->data;
    return *flistsize;
}

FILEINFO *getfileinfolist(int fd, int numfiles){
    FILEINFO *finfolist;
    finfolist = (FILEINFO *) malloc(sizeof(FILEINFO) * numfiles);
    
    DATA *buffer;
    unsigned long *ulptr;
    unsigned long pos = 7;

    int i;
    for(i = 0; i < numfiles; i++){
        buffer = loadchkfile(fd, pos, 256);
        if(buffer == NULL){
            set_errorno(E_CANNOTREADFILE);
            return NULL;
        }
        
        char *ptr;
        ptr = (char *) buffer->data;
        finfolist[i].filename = (char *) malloc(sizeof(char)*256);
        strcpy(finfolist[i].filename, ptr);

        pos = pos + 256;

        buffer = loadchkfile(fd, pos, sizeof(unsigned long));
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

HEADERINFO *getpiuheader(int fd){
    HEADERINFO *header;

    if(fd < 0){ //Check for valid file descriptor
        set_errorno(E_CANNOTREADFILE);
        return NULL;
    }

    if(!ispiufile(fd))
        return NULL;

    header = (HEADERINFO *) malloc(sizeof(HEADERINFO)); 
    
    header->flistsize = getflistsize(fd); /* Retrieves filelist size */
    if(header->flistsize < 0)
        return NULL;
        
    header->filelist.filecount = header->flistsize / FILEINFO_SIZE; /* How many files do we have? */

    header->filelist.fileinfo = getfileinfolist(fd, header->filelist.filecount); /* Retrieving file information list */
    if(header->filelist.fileinfo == NULL)
        return NULL;

    return header;
}


PIUFILE *openpiufile(char *file){
    PIUFILE *piu;
    piu = (PIUFILE *) malloc(sizeof(PIUFILE));

    int fd = fileopen(file, FMUSTEXIST);
    piu->header = getpiuheader(fd);
    close(fd);
    if(piu->header == NULL)
        return NULL;
    unsigned long datapos;
    datapos = HEADER_MINSIZE + piu->header->flistsize; /*It starts pointing to the data start position. */
    
    piu->filedata = (DATA *) realloc(piu->filedata, sizeof(DATA)*(piu->header->filelist.filecount + 1));
    DATA *readbuffer;
    int i;    
    for(i = 0; i < FCOUNT; i++){
        /* Calculate the amount of bytes to skip */
        if(i != 0)
            datapos = datapos + piu->header->filelist.fileinfo[i-1].size;
        /* Reading data section from piu file */    
        fd = fileopen(file, FMUSTEXIST);
        readbuffer = loadchkfile(fd, datapos, piu->header->filelist.fileinfo[i].size);
        if(readbuffer == NULL)
            return NULL; 
        piu->filedata[i].data = readbuffer->data;
        piu->filedata[i].size = readbuffer->size;
        close(fd);
    }

    return piu;
}

PIUFILE *createpiufile(){
    PIUFILE *piu;
    piu = (PIUFILE *) malloc(sizeof(PIUFILE));
    piu->header = (HEADERINFO *) malloc(sizeof(HEADERINFO));
    piu->filedata = NULL;
    piu->header->flistsize = 0;
    piu->header->filelist.filecount = 0;

    return piu;
}

int writepiu(PIUFILE *piu, char *filepath){
    /* Better create some defines for short naming */
    #define FINFO    piu->header->filelist.fileinfo
    #define FCOUNT   piu->header->filelist.filecount
 
    /* Unlink "deletes" the file */
    unlink(filepath);
    
    DATA *writebuf;
    writebuf = (DATA *) malloc(sizeof(DATA));
    
    char format[3] = "PIU";
    writebuf->data = format;
    writebuf->size = 3;
    
    if(savetofile(filepath, writebuf) != 3){
        set_errorno(E_CANNOTWRITEFILE);
        return 0;
    }
    /* Closing the file to commit changes to it, then reopening */
    
    int fd = fileopen(filepath, FCREATE);
    if(fd < 0){
        set_errorno(E_CANNOTOPENFILE);
        return 0;
    }
     
    int hsize = piu->header->flistsize;
    writebuf->data = &hsize;
    writebuf->size = sizeof(int);
    
    if(appendtofd(fd, writebuf) != sizeof(int)){
        set_errorno(E_CANNOTWRITEFILE);
        return 0;
    }
    
    int i;
    for(i = 0; i < FCOUNT; i++){
        writebuf->data = FINFO[i].filename;
        writebuf->size = 256; 
        if(appendtofd(fd, writebuf) != 256){
            set_errorno(E_CANNOTWRITEFILE);
            return 0;
        }

        writebuf->data = &FINFO[i].size;
        writebuf->size = sizeof(unsigned long);
        if(appendtofd(fd, writebuf) != sizeof(unsigned long)){
            set_errorno(E_CANNOTWRITEFILE);
            return 0;
        }
    }

    /* Write file data */
    for(i = 0; i < FCOUNT; i++){
        if(appendtofd(fd, &piu->filedata[i]) != piu->filedata[i].size){
            set_errorno(E_CANNOTWRITEFILE);
            return 0;
        }
    }

    close(fd);
    return 1;
}

int addfile(PIUFILE *piu, char *filepath){
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
    piu->header->flistsize = piu->header->flistsize + FILEINFO_SIZE;
    /* Storing filename into the data structure. We take the name from
     * the filepath parameter.
     * We need to allocate 256 bytes for store the filename.
     */
    FINFO[FCOUNT].filename = (char *) malloc(256);
    PIUSTRING *filename = newpiustring(256);
    strcpy(filename->str, filepath);
    rmparentpathname(filename);
    strcpy(FINFO[FCOUNT].filename, filename->str);
    
    FINFO[FCOUNT].size = filedata->size;

    /* Adding the new file data to the structure */
    /* FIXME: If this sentence fails, we wont know it */ 
    piu->filedata = (DATA *) realloc(piu->filedata,  
                                     sizeof(DATA) * (FCOUNT + 1));
    piu->filedata[FCOUNT] = *filedata;
    FCOUNT = FCOUNT + 1;
    return 1;
}

void unloadpiufile(PIUFILE *piu){

    /* Unload data section */
    /* Unload header section */
}
