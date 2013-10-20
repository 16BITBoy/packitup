#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "piu/piu.hpp"

using namespace PIU;

void printFileInfo(FileInfo file) {
    std::cout << file.filePath << std::endl << file.fileSize << std::endl << std::endl;
}

int main(int argc, char **argv){
    PIUArchive *ar = NULL;
    try{
        if(argc < 3) {
            std::cerr << "Missing parameters" << std::endl;
            return EXIT_FAILURE;
        }
        ar = new PIUArchive(argv[1]);
        ar->addFile(argv[2]);
        std::vector<FileInfo> files = ar->listFiles();
        std::cout << "There are : " << files.size() << " files." << std::endl;
        for_each(files.begin(), files.end(), printFileInfo);
    }
    catch(UndefinedException e){
        std::cout << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
