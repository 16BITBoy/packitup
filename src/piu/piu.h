#ifndef _PIU_H_
#define _PIU_H_

#include "../errors/errors.h"
#include "../fdhandler/fdhandler.h"
#define FILEINFO_SIZE   264
#define HEADER_MINSIZE  7

typedef struct{
    char *filename;
    unsigned long int size;
}FILEINFO;

typedef struct{
    int filecount;
    FILEINFO *fileinfo;
}FILELIST;

typedef struct{
    int flistsize;
    FILELIST filelist;
}HEADERINFO;

typedef struct{
    HEADERINFO *header;
    DATA *filedata;
}PIUFILE;

int ispiufile(int fd);
int getflistsize(int fd);
FILEINFO *getfileinfolist(int fd, int numfiles);
HEADERINFO *getpiuheader(int fd);
int getpiudata(PIUFILE *piu, int fd);
PIUFILE *openpiufile(char *file);
PIUFILE *createpiufile();
int writepiu(PIUFILE *piu, char *filepath);
int addfile(PIUFILE *piu, char *filepath);
#endif /* _PIU_H_ */

