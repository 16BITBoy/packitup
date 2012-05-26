#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <vector>
#include <exception>
#include <boost/filesystem.hpp>

#include "fdhandler/fdhandler.hpp"
#include "tests/test_data.cpp"
#include "tests/tests_fdhandler.cpp"

using namespace std;


namespace PIU{
/** DRAFT: In design process **/

typedef unsigned int FileListSize;
typedef unsigned long int FileSize;
typedef unsigned int FileNameLength;

class FileInfo{
public:
    FileInfo(){};
    // PIU File format fields
    string fileName;
    FileSize fileSize;

    // Not from PIU file
    bool write; // Will be written in new version of this archive.
};

class PIUHeader{
public:
    // PIU File format fields
    FileListSize fileListSize;
    vector<FileInfo> fileList;
};

class PIUArchiveException : public exception{
public:
    const char* what() const throw()
    {
        return "Unknown exception while handling PIU archive.";
    }
};

class InvalidSignatureException : public PIUArchiveException{
public:
    const char* what() const throw()
    {
        return "PIU archive signature invalid.";
    }
};

// Top level class PIUArchive.
class PIUArchive{
private:
    string fileName;
    PIUHeader headerInfo;
    vector<Data> fileData;
public:
    /** \brief Specifies the file to work with and loads the header information **/
    PIUArchive(string fileName) throw(PIUArchiveException,
                                      InvalidSignatureException);
    /** \brief Returns a vector with the information about the files in the archive **/
    vector<FileInfo> listFiles();
    /** \brief Dumps changes to the file on disk. **/
    void write();
    /** \brief Adds a new file to the archive in memory (just adds the header information to the file in memory) **/
    void addFile(string fileName);
    /** \brief Deletes a file from the archive in memory (just deletes the header information to the file in memory) **/
    void deleteFile(string fileName);
    /** \brief Reads the data from the file specified in the archive in memory and writes it into the specified path **/
    void extractFile(string fileName, string extractPath);
};

PIUArchive::PIUArchive(string fileName) throw (PIUArchiveException,
                                               InvalidSignatureException){
    using boost::filesystem::exists;
    using boost::filesystem::path;
    this->fileName = fileName;
    path pathToPIUArchive(fileName.c_str());

    if(!exists(pathToPIUArchive)){
        return; // This will be a new file. Read nothing.
    }

    FDHandler fd(fileName);

    //Check archive format signature
    Data *inBuf = new Data();
    fd.readchk(inBuf, 0, 3);
    string signature;
    int i;
    char *c = NULL;
    for(i = 0; i < 3; i++){
        c = (char *)inBuf->data + i;
        signature.append(1, *c);
    }
    if(signature.compare("PIU") != 0){
        throw InvalidSignatureException();
    }

    //Read file list size
    inBuf->free();
    if(!fd.readchk(inBuf, 3, sizeof(FileListSize)))
        throw PIUArchiveException();
    this->headerInfo.fileListSize = *(FileListSize *)inBuf->data;

    //Read the whole file list and create a pointer "buffer" pointing to the
    //beginning of the list.
    inBuf->free();
    if(!fd.readchk(inBuf, 7, this->headerInfo.fileListSize))
        throw PIUArchiveException();
    char *buffer = (char *)inBuf->data;
    FileListSize bytesRead = 0;

    //Read the contents from the file list and store them.
    while(bytesRead < this->headerInfo.fileListSize){
        FileNameLength fnl = *(FileNameLength *)(buffer + bytesRead);
        bytesRead += sizeof(FileNameLength);
        FileInfo fInfo;
        char *tmpStr = new char[fnl + 1];
        for(i = 0; i < fnl; i++){
            *(tmpStr + i) = *(buffer + bytesRead + i);
        }
        bytesRead += fnl;
        *(tmpStr + fnl) = '\0';
        fInfo.fileName = tmpStr;
        fInfo.fileSize = *(FileSize *)(buffer + bytesRead);
        bytesRead += sizeof(FileSize);
        this->headerInfo.fileList.push_back(fInfo);
    }

}

// TODO
void PIUArchive::write(){
    using boost::filesystem::exists;
    using boost::filesystem::path;
    string tmpFileName = this->fileName + ".tmp";
    FDHandler fNew(tmpFileName);
    FDHandler fOld(this->fileName);
    PIUArchive *oldPIUAr = NULL;
    path oldPIUArPath(this->fileName.c_str());

    if(exists(oldPIUArPath)){
        oldPIUAr = new PIUArchive(this->fileName);

    }


}

vector<FileInfo> PIUArchive::listFiles(){
    return this->headerInfo.fileList;
}
/** END DRAFT **/
}


int main(int argc, char **argv){
    PIU::PIUArchive *ar = NULL;
    try{
        ar = new PIU::PIUArchive("modelo");
    }
    catch(PIU::PIUArchiveException &e){
        cout << e.what() << endl;
    }
    vector<PIU::FileInfo> files = ar->listFiles();
    int i;
    for(i = 0; i < files.size(); i++){
        cout << files[i].fileName << "  " << files[i].fileSize << endl;
    }
    ar->write();
    cout << "Ejecución finalizada." << endl;
    return 0;
}
