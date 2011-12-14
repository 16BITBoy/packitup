#ifndef _PIU_H_
#define _PIU_H_

#include "../errors/errors.h"
#include "../fdhandler/fdhandler.h"
#define FILEINFO_SIZE   264
#define HEADER_MINSIZE  7

typedef struct{
    char *str;
    int len;
}PIUSTRING;

typedef struct{
    PIUSTRING *items;
    int nitems;
}PIUSTRARR;

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
PIUSTRING *concatpiustrarr(PIUSTRARR *strs);
PIUSTRARR *piustrsplit(PIUSTRING *str, char sep);
PIUSTRING *rmparentpathname(PIUSTRING *path);
int createpath(PIUSTRING *path);
PIUSTRING *newpiustring(int len);
PIUSTRARR *newpiustrarr();
PIUSTRARR *addpiustrarritem(PIUSTRARR *strs, PIUSTRING *str);
int substr(char *dest, char *src, int start, int end);
int ispiufile(int fd);
int getflistsize(int fd);
FILEINFO *getfileinfolist(int fd, int numfiles);
HEADERINFO *getpiuheader(int fd);
int getpiudata(PIUFILE *piu, int fd);
PIUFILE *openpiufile(char *file);
PIUFILE *createpiufile();
int writepiu(PIUFILE *piu, char *filepath);
int addfile(PIUFILE *piu, char *filepath);
int deletefile(PIUFILE *piu, unsigned long filenum);
#endif /* _PIU_H_ */

