#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

#include "../piu/piu.hpp"

using namespace PIU;

// Shows all the information about the PIU file on screen
void showPIUFileInformation(std::string file) throw (UndefinedException) {
    PIUArchive *ar = NULL;
    ar = new PIUArchive(file);
    std::vector<FileInfo> fileList = ar->listFiles();
    int i;
    std::cout << "File information read from PIU archive:" << std::endl;
    std::cout << "File list size: " << ar->getFileListSize() << std::endl;
    for(i = 0; i < fileList.size(); i++){
        std::cout << fileList[i].filePath << " : "
                  << fileList[i].fileSize << std::endl;
    }

}

int main(int argc, char **argv) {
    try {
        if(argc < 2){
            std::cout << "File not specified. \n" << std::endl;
            return EXIT_FAILURE;
        }
        showPIUFileInformation(std::string(argv[1]));
        return EXIT_SUCCESS;
        }
    catch(UndefinedException &e){
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

