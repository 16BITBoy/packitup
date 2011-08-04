#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piu/piu.h"

/* LIST OF TESTS FUNCTIONS 
 *
 * Only for testing purposes, this is not part of PackItUp
 *
 * */
int main(int argc, char **argv){
    PIUFILE *piu = openpiufile("piufile");
    if(!addfile(piu, "afile")){
        printf("\nEl fichero no ha podido ser anyadido a este paquete\n");
        return 1;
    }
    printf("\nNuevo fichero anyadido al paquete\n");
    if(!writepiu(piu, "piufile")){
        printf("\nEl fichero no se ha podido escribir correctamente.\n");
        return 1;
    }
    printf("\nEl fichero se ha escrito con exito\n");
    return 0; 
}
