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

int is_piu_file(int fd);
int get_flistsize(int fd);
FILEINFO *get_fileinfolist(int fd, int numfiles);
HEADERINFO *get_piu_header(int fd);
PIUFILE *open_piu_file(char *file);

#endif /* _PIU_H_ */

