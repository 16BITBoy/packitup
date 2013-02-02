/*
 * piu.cpp
 *
 *  Created on: 26/05/2012
 *      Author: Adrián Pérez Heredia
 */

#include <boost/filesystem.hpp>
#include <iomanip>
#include <vector>
#include <string>

#include "piu.hpp"

namespace PIU{

void PIUArchive::getHeaderInfo() throw(PIUArchiveException,
                                       InvalidSignatureException){
    FDHandler fd(this->fileName);

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

    /* TODO: This '3' below is the position of the filelistsize in a PIU file
       Thus, it must be a defined constant. */
    if(!fd.readchk(inBuf, 3, sizeof(FileListSize)))
        throw PIUArchiveException();
    this->headerInfo.fileListSize = *(FileListSize *)inBuf->data;

    //Read the whole file list and create a pointer "buffer" pointing to the
    //beginning of the list.
    inBuf->free();

    /* TODO: This '7' below is the position of the beginning of the file list
       Thus, it must be a defined constant. */
    if(!fd.readchk(inBuf, 7, this->headerInfo.fileListSize))
        throw PIUArchiveException();
    char *buffer = (char *)inBuf->data;
    FileListSize bytesRead = 0;
    FileListSize pos = 0;
    //Read the contents from the file list and store them.
    while(bytesRead < this->headerInfo.fileListSize){
        FileNameLength fnl = *(FileNameLength *)(buffer + bytesRead);
        bytesRead += sizeof(FileNameLength);
        FileInfo fi;
        char *tmpStr = new char[fnl + 1];
        for(i = 0; i < fnl; i++){
            *(tmpStr + i) = *(buffer + bytesRead + i);
        }
        bytesRead += fnl;
        *(tmpStr + fnl) = '\0';
        fi.fileName = tmpStr;
        fi.fileSize = *(FileSize *)(buffer + bytesRead);
        bytesRead += sizeof(FileSize);
        this->headerInfo.fileList.push_back(fi);
        // Also, set to true filesKept value so the file will be copied by default
        // in next version of this PIU archive, unless the opposite is explicitly specified.
        this->ops.filesKept.push_back(true);
        // Map the filename to the position this file will be kept in the fileList vector.
        this->posMap[fi.fileName] = pos;
        ++pos;
    }
}

void PIUArchive::updateHeaderInfo() throw(PIUArchiveException,
                                     InvalidSignatureException){
    this->headerInfo.fileList.clear();
    this->headerInfo.fileListSize = 0;
    getHeaderInfo();
}

PIUArchive::PIUArchive(std::string fileName) throw (PIUArchiveException,
                                               InvalidSignatureException,
                                               PIUArchiveEmptyFileName){
    using boost::filesystem::exists;
    using boost::filesystem::path;
    if(fileName.empty())
        throw PIUArchiveEmptyFileName();

    this->fileName = fileName;
    path pathToPIUArchive(fileName.c_str());

    if(!exists(pathToPIUArchive)){
        return; // This will be a new file. Read nothing.
    }
    getHeaderInfo();
}

unsigned long int PIUArchive::getFileOffset(int position){
    FileSize offInFileDataSpace = 0;
    FileListSize i;
    for(i = 0; i < position; i++){
        offInFileDataSpace += this->headerInfo.fileList[i].fileSize;
    }
    return PIU_SIGNATURE_SIZE + sizeof(FileListSize) + this->headerInfo.fileListSize + offInFileDataSpace;
}

/* TODO: Calling this function require to update all the header information from the new
         on disk version */
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
    fNew.write((char *)PIU_SIGNATURE.c_str(), 3);
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
    // Copy data from new files into the archive
    FDHandler *file = NULL;
    for(i = 0; i < this->ops.newFiles.size(); i++){
        file = new FDHandler(this->ops.newFiles[i]);
        if(!file->readall(buf)){
            throw PIUArchiveException();
        }
        fNew.append(buf);
        buf->free();
        delete file;
    }
    // Remove the old file and rename the temp file to the name the old file had.
    boost::filesystem::remove(oldPIUArPath);
    boost::filesystem::path tmpFilePath(tmpFileName.c_str());
    boost::filesystem::rename(tmpFilePath, oldPIUArPath);
    updateHeaderInfo();
}

/* TODO: Calling this function will require to recalculate this->headerInfo.fileListSize after
   the call to keep it updated. */
void PIUArchive::deleteFile(std::string fileName){
    FileListSize pos = this->posMap.at(fileName);
    this->headerInfo.fileList.erase(this->headerInfo.fileList.begin() + pos);
    posMap.erase(fileName);
    this->ops.filesKept[pos] = false;
    computeFileListSize();
}

void PIUArchive::computeFileListSize(){
    unsigned int fileCount = this->headerInfo.fileList.size();
    FileListSize totalSize = (sizeof(FileSize) + sizeof(FileNameLength)) * fileCount;
    int i;
    for(i = 0; i < this->headerInfo.fileList.size(); i++){
        totalSize += this->headerInfo.fileList[i].fileName.length();
    }
    this->headerInfo.fileListSize = totalSize;
}

std::vector<FileInfo> PIUArchive::listFiles(){
    return this->headerInfo.fileList;
}

FileListSize PIUArchive::getFileListSize(){
    return this->headerInfo.fileListSize;
}

/** END DRAFT **/
} // end namespace PIU
