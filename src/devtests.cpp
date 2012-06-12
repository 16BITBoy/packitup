#include <iostream>
#include <vector>
#include <string>
#include <boost/unordered_map.hpp>

#include "fdhandler/fdhandler.hpp"
#include "piu/piu.hpp"

using namespace PIU;
int main(int argc, char **argv){
    PIUArchive *ar = NULL;
    try{
        ar = new PIUArchive("modelo.tmp");
    }
    catch(PIUArchiveException &e){
        std::cout << e.what() << std::endl;
    }
    std::vector<FileInfo> fileList = ar->listFiles();
    int i;
    std::cout << "File information read from original PIU archive:" << std::endl;
    for(i = 0; i < fileList.size(); i++){
        std::cout << fileList[i].fileName << " : " << fileList[i].fileSize << std::endl;
    }

    ar->write();

    return 0;
}
