#include <iostream>
#include <vector>
#include <string>

#include "piu/piu.hpp"

using namespace PIU;


int main(int argc, char **argv){
    PIUArchive *ar = NULL;
    try{
        ar = new PIUArchive("test");
        ar->addFile("b");
    }
    catch(UndefinedException e){
        std::cout << e.what() << std::endl;
    }


    return EXIT_SUCCESS;
}
