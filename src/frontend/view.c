#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../piu/piu.h"


void show_help(){
    printf("\nUsage: piuview <filename>\n\n");
}

int main(int argc, char **argv){
    /*FIXME: Look for a way to know if the file exists*/
    if(argc < 2){
        show_help();
        return 1;
    }
    if(strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0){
        printf("\npiuview - Pack It Up file viewer\nver:0.1.3\n\n");
        return 0;
    }
    int file = fileopen(argv[1], 0);
    HEADERINFO *header = getpiuheader(file);
    if(header == NULL){
        printf("%s\n", piu_errmsg(piu_errno));
        return piu_errno;
    }

    printf(":::     PackItUp File Viewer     :::\n");
    printf("------------------------------------\n");
    printf("Header size (bytes): %d\n\n", header->flistsize);
    printf("-------------Files------------------\n");
    printf("Id\tSize\t\tFilename\n");
    
    if(header->filelist.filecount == 0){
        printf("\nThis file is empty\n\n");
        return 0;
    }
    int i;
    for(i = 0; i < header->filelist.filecount; i++){
        printf("%d\t%ld\t\t%s\n", i,
                                  header->filelist.fileinfo[i].size, 
                                  header->filelist.fileinfo[i].filename);
    }
   
    return 0;
}

