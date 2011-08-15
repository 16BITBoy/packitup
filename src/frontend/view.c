#include <stdio.h>
#include <stdlib.h>
#include "../piu/piu.h"


void show_help(){
    printf("\n\nUsage: piuview <filename>\n\n");
}

int main(int argc, char **argv){
    /*FIXME: Look for a way to know if the file exists*/

    if(argc < 2){
        show_help();
        return 1;
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
    printf("Filename\tSize\n");
    int i;

    if(header->filelist.filecount == 0){
        printf("\nThis file is empty\n\n");
        return 0;
    }

    for(i = 0; i < header->filelist.filecount; i++){
        printf("%s\t%ld\n",header->filelist.fileinfo[i].filename, header->filelist.fileinfo[i].size);
    }
   
    return 0;
}

