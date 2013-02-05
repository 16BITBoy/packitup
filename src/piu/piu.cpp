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

/** BEGIN DRAFT **/

void PIUArchive::getHeaderInfo() throw(PIUArchiveException,
                                       FileFormatError){
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
        throw FileFormatError("Bad piu archive signature exception.");
    }

    //Read file list size
    inBuf->free();

    /* TODO: This '3' below is the position of the filelistsize in a PIU file
       Thus, it must be a defined constant. */
    if(!fd.readchk(inBuf, 3, sizeof(FileListSize)))
        throw PIUArchiveException("Read error while trying to get the size of the file list.");
    this->headerInfo.fileListSize = *(FileListSize *)inBuf->data;

    //Read the whole file list and create a pointer "buffer" pointing to the
    //beginning of the list.
    inBuf->free();

    /* TODO: This '7' below is the position of the beginning of the file list
       Thus, it must be a defined constant. */
    if(!fd.readchk(inBuf, 7, this->headerInfo.fileListSize))
        throw PIUArchiveException("Read error while trying to read the information from the file list.");
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
                                     FileFormatError){
    this->headerInfo.fileList.clear();
    this->headerInfo.fileListSize = 0;
    getHeaderInfo();
}

PIUArchive::PIUArchive(std::string fileName) throw (PIUArchiveException,
                                                    BadParameter){
    using boost::filesystem::exists;
    using boost::filesystem::path;
    if(fileName.empty())
        throw BadParameter("PIUArchive constructor called with an empty string.");

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

/* TODO: Calling this function will require to update all the header information from the new
         on disk version
 */
void PIUArchive::write() throw(PIUArchiveException,
                               FileNotFound){
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
            throw FileNotFound("PIU archive has been moved or doesn't exist.");
        oldPIUAr = new PIUArchive(this->fileName);
        for(i=0; i<ops.filesKept.size(); i++){
            if(ops.filesKept[i]){
                FileSize oldOff = oldPIUAr->getFileOffset(i);
                // DEBUG: cout << "File data is at address :" << hex << oldOff << endl;
                if(!fOld.readchk(buf, oldOff, oldPIUAr->headerInfo.fileList[i].fileSize))
                    throw PIUArchiveException("Error trying to read file data from PIU archive.");
                std::cout << std::dec;
                if(buf->size != oldPIUAr->headerInfo.fileList[i].fileSize){
                    std::cout << "ERROR: " << std::dec << buf->size << " bytes read from old PIU file. "
                              << headerInfo.fileList[i].fileSize - buf->size << " bytes missing."
                              << std::endl;
                    throw PIUArchiveException("Error reading file data. There are bytes missing. Not enough memory perhaps?");
                }
                std::cout << buf->size << " bytes successfully read from old PIU file." << std::endl;
                if(!fNew.append(buf)){
                    throw PIUArchiveException("Error trying to write data. Not enough free space on disk perhaps?");
                }
                buf->free();
            }
        }
    }
    // Copy data from new files to add into the archive
    FDHandler *file = NULL;
    for(i = 0; i < this->ops.newFiles.size(); i++){
        file = new FDHandler(this->ops.newFiles[i]);
        if(!file->readall(buf)){
            throw PIUArchiveException("Error reading file \""+this->ops.newFiles[i]+"\".");
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
/**
 * TODO: Store the gatered information in headerInfo. Don't forget to create
 * the needed entry in posMap.
 **/
void PIUArchive::addFile(std::string fileName) throw(PIUArchiveException,
                                                     FileNotFound,
                                                     BadParameter){
    if(fileName.empty()){
        throw BadParameter("Called PIUArchive::addFile() with empty string.");
    }

    boost::filesystem::path filePath(fileName.c_str());
    boost::uintmax_t fileSize;

    try{
        fileSize = boost::filesystem::file_size(filePath);
    }
    catch(boost::filesystem::filesystem_error e){
        throw FileNotFound("File \""+fileName+"\" not found.");
    }

    if(fileSize < 0){
        std::ostringstream converter;
        converter << fileSize;
        throw PIUArchiveException("Unexpected file size found. Filename: "+fileName+" | File size: "+converter.str());
    }

}

/** END DRAFT **/
} // end namespace PIU
