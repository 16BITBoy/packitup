#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <vector>

#include "fdhandler/fdhandler.hpp"
#include "tests/test_data.cpp"
#include "tests/tests_fdhandler.cpp"

using namespace std;

/** DRAFT: In design process **/

typedef unsigned int FileListSize;
typedef unsigned long int FileSize;
typedef unsigned int FileNameLength;

typedef struct FileInfo{
public:
    string fileName;
    FileSize fileSize;
};

typedef struct PIUHeader{
public:
    FileListSize fileListSize;
    vector<FileInfo> fileList;
};

// Top level class PIUArchive.
class PIUArchive{
private:
    string fileName;
    PIUHeader headerInfo;
    vector<Data> fileData;
public:
    PIUArchive(string fileName);

    vector<FileInfo> listFiles();
    void write();
    void addFile(string fileName);
    void deleteFile(string fileName);
    void extractFile(string fileName, string extractPath);
};

PIUArchive::PIUArchive(string fileName){
    this->fileName = fileName;

}
/** END DRAFT **/



int main(int argc, char **argv){

    return 0;
}
