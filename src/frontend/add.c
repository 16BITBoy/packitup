#include <stdio.h>
#include <stdlib.h>
#include "../piu/piu.h"

int main(int argc, char **argv){
    if(argc < 3){
        printf("\nUsage: piuadd PIUFILE FILE...");
        exit(1);
    }
    /* Load PIU file and the file to be added */
    PIUFILE *piu = openpiufile(argv[1]);
    if(piu == NULL){
        exit(2);
    }
    
    int i;
    float prog = 0;
    printf("\nAdding new files...\n");
    for(i = 2; i < argc; i++){
        prog = (100 / (argc - 1)) * (i - 1);
        printf("\n%f ...\n", prog);
        if(!addfile(piu, argv[i])){
            set_errorno(E_CANNOTREADFILE);
            printf("\n%s\n",piu_errmsg(piu_errno));
            exit(3);
        }
    }
    printf("\n100\% ...\n"); 
    printf("\n\nWriting file...\n\n");
    if(!writepiu(piu, argv[1])){
        set_errorno(E_CANNOTWRITEFILE);
        printf("\n%s\n",piu_errmsg(piu_errno));
        exit(4);
    }
    
    return 0;
}

