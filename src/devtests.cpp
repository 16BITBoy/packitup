#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <vector>
#include <exception>
#include <iomanip>
#include <boost/filesystem.hpp>

#include "fdhandler/fdhandler.hpp"

namespace PIU{

/** DRAFT: In design process **/

typedef unsigned int FileListSize;
typedef unsigned long int FileSize;
typedef unsigned int FileNameLength;

const unsigned int SIGNATURE_SIZE = 3;
std::string SIGNATURE = "PIU";

class FileInfo{
public:
    FileInfo(){};
    // PIU File format fields
    std::string fileName;
    FileSize fileSize;
};

class PIUHeader{
public:
    // PIU File format fields
    FileListSize fileListSize;
    std::vector<FileInfo> fileList;
};

class PIUArchiveException : public std::exception{
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

class PIUArchiveDoesNotExists : public PIUArchiveException{
    const char* what() const throw(){
        return "PIU archive doesn't exists.";
    }
};

class Operations{
public:
    std::vector<std::string> newFiles; // New files to be written into the new PIU archive.
    std::vector<bool> filesKept; /* Files to be written from the old PIU archive to the new one.
                               The position of every boolean value in the vector corresponds
                               to the position of every file in the old PIU archive.
                            */
};


// Top level class PIUArchive.
class PIUArchive{
private:
    std::string fileName;
    PIUHeader headerInfo;
    std::vector<Data> fileData;
    Operations ops; // Operations to be done in this PIU archive.
    unsigned long int getFileOffset(int position);
public:
    /** \brief Specifies the file to work with and loads the header information **/
    PIUArchive(std::string fileName) throw(PIUArchiveException,
                                      InvalidSignatureException);
    /** \brief Returns a vector with the information about the files in the archive **/
    std::vector<FileInfo> listFiles();
    /** \brief Dumps changes to the file on disk. **/
    void write() throw(PIUArchiveException,
                       PIUArchiveDoesNotExists);
    /** \brief Adds a new file to the archive in memory (just adds the header information to the file in memory) **/
    void addFile(std::string fileName);
    /** \brief Deletes a file from the archive in memory (just deletes the header information to the file in memory) **/
    void deleteFile(std::string fileName);
    /** \brief Reads the data from the file specified in the archive in memory and writes it into the specified path **/
    void extractFile(std::string fileName, std::string extractPath);
};
}





namespace PIU{


PIUArchive::PIUArchive(std::string fileName) throw (PIUArchiveException,
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
    std::string signature;
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
        this->ops.filesKept.push_back(true);
    }

}

unsigned long int PIUArchive::getFileOffset(int position){
    FileSize offInFileDataSpace = 0;
    FileListSize i;
    for(i = 0; i < position; i++){
        offInFileDataSpace += this->headerInfo.fileList[i].fileSize;
    }
    return SIGNATURE_SIZE + sizeof(FileListSize) + this->headerInfo.fileListSize + offInFileDataSpace;
}

// TODO
void PIUArchive::write() throw(PIUArchiveException,
                               PIUArchiveDoesNotExists){
    using boost::filesystem::exists;
    using boost::filesystem::path;
    std::string tmpFileName = this->fileName + ".tmp";
    FDHandler fNew(tmpFileName);
    FDHandler fOld(this->fileName);
    PIUArchive *oldPIUAr = NULL;
    path oldPIUArPath(this->fileName.c_str());

    /* Writing new PIU archive header
     */
    fNew.write((char *)SIGNATURE.c_str(), 3);
    fNew.append((char *)&this->headerInfo.fileListSize, sizeof(FileListSize));
    int i;
    for(i = 0; i < this->headerInfo.fileList.size(); i++){
        FileNameLength fnl = this->headerInfo.fileList[i].fileName.length();
        fNew.append((char *)&fnl, sizeof(FileNameLength));
        fNew.append((char *)this->headerInfo.fileList[i].fileName.c_str(), this->headerInfo.fileList[i].fileName.length());
        fNew.append((char *)&this->headerInfo.fileList[i].fileSize, sizeof(FileSize));
    }

    /* Writing file data
     */
    Data *buf = new Data();
    // If this is not a new PIU file, copy the files
    // to be kept from the old version to the new one of the PIU Archive.
    if(this->ops.filesKept.size() > 0){
        if(!exists(oldPIUArPath))
            throw PIUArchiveDoesNotExists();
        oldPIUAr = new PIUArchive(this->fileName);
        for(i=0; i<ops.filesKept.size(); i++){
            if(ops.filesKept[i]){
                FileSize oldOff = oldPIUAr->getFileOffset(i);
                // DEBUG: cout << "File data is at address :" << hex << oldOff << endl;
                if(!fOld.readchk(buf, oldOff, oldPIUAr->headerInfo.fileList[i].fileSize))
                    throw PIUArchiveException();
                std::cout << std::dec;
                if(buf->size != oldPIUAr->headerInfo.fileList[i].fileSize){
                    std::cout << "ERROR: " << std::dec << buf->size << " bytes read from old PIU file. "
                         << headerInfo.fileList[i].fileSize - buf->size << " bytes missing."
                         << std::endl;
                    throw PIUArchiveException();
                }
                std::cout << buf->size << " bytes successfully read from old PIU file." << std::endl;
                if(!fNew.append(buf)){
                    throw PIUArchiveException();
                }
                buf->free();
            }
        }
    }
    // TODO: Write the new files into the archive.


}

std::vector<FileInfo> PIUArchive::listFiles(){
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
        std::cout << e.what() << std::endl;
    }
    std::vector<PIU::FileInfo> fileList = ar->listFiles();
    int i;
    std::cout << "File information read from original PIU archive:" << std::endl;
    for(i = 0; i < fileList.size(); i++){
        std::cout << fileList[i].fileName << " : " << fileList[i].fileSize << std::endl;
    }
    ar->write();

    return 0;
}
