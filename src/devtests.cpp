#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <boost/filesystem.hpp>

#include "piu/piu.hpp"

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
        boost::filesystem::path packagePath(argv[1]);
        if(!boost::filesystem::exists(packagePath)) {
            std::cerr << "Package file \""+package+"\" doesn't exist." << std::endl;
            return EXIT_FAILURE;
        }
        ar = new PIUArchive(package);
        if(!ar->exists(file)) {
            std::cerr << "File \""+file+"\" doesn't exist inside the specified package file." << std::endl;
            return EXIT_FAILURE;
        }
        ar->extractFile(file);
        return EXIT_SUCCESS;
    }
    catch(UndefinedException e){
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }



}
