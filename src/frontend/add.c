#include <stdio.h>
#include <stdlib.h>
#include "../piu/piu.h"

int main(int argc, char **argv){
    if(wrongparameters()){
        exit(1);
    }
    /* Load PIU file and the file to be added */
    DATA *piufile = open_piu_file(argv[1]);
    if(piufile == NULL){
        exit(2);
    }

    DATA *file = load_file(argv[2]);
    if(file == NULL){
        exit(2);
    }
    /* design still not complete */
    return 0;
}



