#include <iostream>
#include <vector>
#include <string>

#include "../piu/piu.hpp"

using namespace PIU;

// Shows all the information about the PIU file on screen
void showPIUFileInformation(std::string file){
    PIUArchive *ar = NULL;
    try{
        ar = new PIUArchive(file);
    }
    catch(UndefinedException &e){
        std::cout << e.what() << std::endl;
    }
    std::vector<FileInfo> fileList = ar->listFiles();
    int i;
    std::cout << "File information read from PIU archive:" << std::endl;
    std::cout << "File list size: " << ar->getFileListSize() << std::endl;
    for(i = 0; i < fileList.size(); i++){
        std::cout << fileList[i].fileName << " : "
                  << fileList[i].fileSize << std::endl;
    }
}

int main(int argc, char **argv){
    if(argc < 2){
        std::cout << "File not specified. \n" << std::endl;
        exit(EXIT_FAILURE);
    }
    showPIUFileInformation(std::string(argv[1]));
    return EXIT_SUCCESS;
}

