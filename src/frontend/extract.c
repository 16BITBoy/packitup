#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "../piu/piu.h"

int main(int argc, char **argv){
    if(argc < 2){
        printf("error: Not enough parameters.\n");
        printf("usage: piuextract [OPTION]\n       piuextract PIUFILE DESTINO\n\n");
        return 1;
    }
    if(strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0){
        printf("piuextract - Pack It Up file extractor\nver:0.1.2\n\n");
        return 0;
    }
    if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
        printf("usage: piuextract [OPTION]\n       piuextract PIUFILE DESTINO\n\n");
        return 0;
    }
    PIUFILE *piu = openpiufile(argv[1]);
    if(piu == NULL){
        printf("\nerror: Cannot read or open piufile\n\n");
        perror("opening file");
        exit(2);
    }

    unsigned long i;
    PIUSTRING *path = NULL;
    PIUSTRARR *dirs = NULL;
    struct stat st;
    for(i = 0; i < piu->header->filelist.filecount; i++){
        path = newpiustring(strlen(piu->header->filelist.fileinfo[i].filename));
        strcpy(path->str, piu->header->filelist.fileinfo[i].filename);
        dirs = piustrsplit(path, '/');
        dirs->nitems--; /* Exclude last item, so we get the parent directory of the file */
        path = concatpiustrarr(dirs);
        if(stat(path->str, &st) < 0 && errno == ENOENT){
            if(!createpath(path)){
                set_errorno(E_CANNOTWRITEFILE);
                perror("error creando ruta");
                printf("Ruta: %s\n", path->str);
                exit(1);
            }
        }
        long bytescopied = savetofile(piu->header->filelist.fileinfo[i].filename,
                                               &piu->filedata[i]);
        if(bytescopied != piu->header->filelist.fileinfo[i].size){
            printf("No se pudo extraer la totalidad de los ficheros\n");
            exit(1);
        }
    }
    return 0;
}
