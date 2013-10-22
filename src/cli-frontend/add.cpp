#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "../piu/piu.hpp"

using namespace PIU;

void printFileInfo(FileInfo file) {
    std::cout << file.filePath << " : " << file.fileSize << std::endl;
}

int main(int argc, char **argv){
    PIUArchive *ar = NULL;
    try{
        if(argc < 3) {
            std::cerr << "Missing parameters" << std::endl;
            return EXIT_FAILURE;
        }
        std::string package = argv[1];
        std::string file = argv[2];
        ar = new PIUArchive(package);
        ar->add(file);
        std::vector<FileInfo> files = ar->listFiles();
        std::cout << "There are : " << files.size() << " files." << std::endl;
        for_each(files.begin(), files.end(), printFileInfo);
        ar->write();
        return EXIT_SUCCESS;
    }
    catch(UndefinedException e){
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }



}
