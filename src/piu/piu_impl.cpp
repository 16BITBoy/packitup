/*
 * piu.cpp
 *
 *  Created on: 26/05/2012
 *      Author: Adrián Pérez Heredia
 */

#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <iomanip>
#include <vector>
#include <string>

#include "piu.hpp"
#include "piu_impl.hpp"
#include "../fdhandler/fdhandler.hpp"

namespace PIU{

/** BEGIN DRAFT **/

void PIUArchiveImpl::getHeaderInfo() throw(UndefinedException,
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
        throw UndefinedException("Read error while trying to get the size of the file list.");
    this->headerInfo.fileListSize = *(FileListSize *)inBuf->data;

    //Read the whole file list and create a pointer "buffer" pointing to the
    //beginning of the list.
    inBuf->free();

    /* TODO: This '7' below is the position of the beginning of the file list
       Thus, it must be a defined constant. */
    if(!fd.readchk(inBuf, 7, this->headerInfo.fileListSize))
        throw UndefinedException("Read error while trying to read the information from the file list.");
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
        fi.filePath = tmpStr;
        fi.fileSize = *(FileSize *)(buffer + bytesRead);
        bytesRead += sizeof(FileSize);
        this->headerInfo.fileList.push_back(fi);
        // Also, set to true filesKept value so the file will be copied by default
        // in next version of this PIU archive, unless the opposite is explicitly specified.
        this->ops.filesKept.push_back(true);
        // Map the filename to the position this file will be kept in the fileList vector.
        this->posMap[fi.filePath] = pos;
        ++pos;
    }
}

void PIUArchiveImpl::updateHeaderInfo() throw(UndefinedException,
                                     FileFormatError){
    this->headerInfo.fileList.clear();
    this->headerInfo.fileListSize = 0;
    getHeaderInfo();
}

PIUArchiveImpl::PIUArchiveImpl(std::string &fileName) throw (UndefinedException,
                                                            BadParameter){
    try{
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
    } catch (boost::filesystem::filesystem_error &e) {
        std::cerr << e.what() << std::endl;
    }
}

unsigned long int PIUArchiveImpl::getFileOffset(int position){
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
void PIUArchiveImpl::write() throw(UndefinedException,
                               FileNotFound){
    std::string tmpFileName = this->fileName + ".tmp";
    FDHandler fNew(tmpFileName);
    FDHandler fOld(this->fileName);
    PIUArchiveImpl *oldPIUAr = NULL;
    boost::filesystem::path oldPIUArPath(this->fileName.c_str());

    /* Writing new PIU archive header
     */
    fNew.write((char *)PIU_SIGNATURE.c_str(), 3);
    fNew.append((char *)&this->headerInfo.fileListSize, sizeof(FileListSize));
    int i;
    for(i = 0; i < this->headerInfo.fileList.size(); i++){
        FileNameLength fnl = this->headerInfo.fileList[i].filePath.length();
        fNew.append((char *)&fnl, sizeof(FileNameLength));
        fNew.append((char *)this->headerInfo.fileList[i].filePath.c_str(), this->headerInfo.fileList[i].filePath.length());
        fNew.append((char *)&this->headerInfo.fileList[i].fileSize, sizeof(FileSize));
    }

    /* Writing file data
     */
    Data *buf = new Data();
    // If this is not a new PIU file, copy the files
    // to be kept from the old version to the new one of the PIU Archive.
    if(this->ops.filesKept.size() > 0){
        if(!boost::filesystem::exists(oldPIUArPath))
            throw FileNotFound("PIU archive has been moved or doesn't exist.");
        oldPIUAr = new PIUArchiveImpl(this->fileName);
        for(i=0; i<ops.filesKept.size(); i++) {
            if(ops.filesKept[i]){
                FileSize oldOff = oldPIUAr->getFileOffset(i);
                // DEBUG: cout << "File data is at address :" << hex << oldOff << endl;
                if(!fOld.readchk(buf, oldOff, oldPIUAr->headerInfo.fileList[i].fileSize))
                    throw UndefinedException("Error trying to read file data from PIU archive.");
                std::cout << std::dec;
                if(buf->size != oldPIUAr->headerInfo.fileList[i].fileSize){
                    std::cout << "ERROR: " << std::dec << buf->size << " bytes read from old PIU file. "
                              << headerInfo.fileList[i].fileSize - buf->size << " bytes missing."
                              << std::endl;
                    throw UndefinedException("Error reading file data. There are bytes missing. Not enough memory perhaps?");
                }
                // DEBUG: std::cout << buf->size << " bytes successfully read from old PIU file." << std::endl;
                if(!fNew.append(buf)){
                    throw UndefinedException("Error trying to write data. Not enough free space on disk perhaps?");
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
            throw UndefinedException("Error reading file \""+this->ops.newFiles[i]+"\".");
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
void PIUArchiveImpl::deleteFile(const std::string &fileName){
    FileListSize pos = this->posMap.at(fileName);
    this->headerInfo.fileList.erase(this->headerInfo.fileList.begin() + pos);
    posMap.erase(fileName);
    this->ops.filesKept[pos] = false;
    computeFileListSize();
}

void PIUArchiveImpl::computeFileListSize(){
    unsigned int fileCount = this->headerInfo.fileList.size();
    FileListSize totalSize = (sizeof(FileSize) + sizeof(FileNameLength)) * fileCount;
    int i;
    for(i = 0; i < this->headerInfo.fileList.size(); i++){
        totalSize += this->headerInfo.fileList[i].filePath.length();
    }
    this->headerInfo.fileListSize = totalSize;
}

std::vector<FileInfo> PIUArchiveImpl::listFiles(){
    return this->headerInfo.fileList;
}

FileListSize PIUArchiveImpl::getFileListSize(){
    return this->headerInfo.fileListSize;
}

/**
 * TODO: make better error handling and make it avoid adding
 *       a file that is already present. If the user wants to update the file
 *       it has first to delete the existing one.
 **/
void PIUArchiveImpl::addFile(boost::filesystem::path &file) throw(UndefinedException,
                                                                  FileNotFound){

    try{
        boost::filesystem::path relativePath;
        relativePath = makeRelativePath(boost::filesystem::current_path(), file).string();
        if(exists(relativePath.string())) {
            std::cout << "File \""+relativePath.string()+"\" already exists in package. Replacing it." << std::endl;
            deleteFile(relativePath.string());
        }

        boost::uintmax_t fileSize = boost::filesystem::file_size(file);
        FileInfo info;
        info.filePath = relativePath.string();
        info.fileSize = fileSize;
        this->headerInfo.fileList.push_back(info);

        this->posMap[relativePath.string()] = this->headerInfo.fileList.size() - 1;
        this->ops.newFiles.push_back(file.string());
        computeFileListSize();
    }
    catch(boost::filesystem::filesystem_error e){
        throw FileNotFound("File \""+file.string()+"\" was reported to exist but now is not found.");
    }

}

void PIUArchiveImpl::addDirectory(boost::filesystem::path &dirPath) throw (UndefinedException,
                                                                           FileNotFound) {
    try {
        if(boost::filesystem::exists(dirPath)) {
            boost::filesystem::directory_iterator dirIt(dirPath);
            boost::filesystem::directory_iterator dirEnd;
            for(; dirIt != dirEnd; dirIt++) {
                boost::filesystem::directory_entry entry;
                entry = *dirIt;
                add(entry.path());
            }
        } else {
            throw FileNotFound("The directory at: "+dirPath.string()+" was reported to exist but now it doesn't.");
        }
    } catch (boost::filesystem::filesystem_error &e) {
        throw UndefinedException(e.what());
    }
}

void PIUArchiveImpl::add(boost::filesystem::path path) throw(UndefinedException,
                                                             FileNotFound) {
    try {
        path = boost::filesystem::canonical(path);
        if(boost::filesystem::exists(path)) {
            if(boost::filesystem::is_directory(path)) {
                addDirectory(path);
            }
            else if(boost::filesystem::is_regular_file(path)) {
                addFile(path);
            }
            else {
                std::cerr << "[WARNING]: Symlink at "+path.string() + " will be ignored." << std::endl;
            }
        } else {
            throw FileNotFound("File \""+path.string()+"\" was reported to exist but now is not found");
        }
    } catch (boost::filesystem::filesystem_error &e) {
        throw UndefinedException(e.what());
    }
}

void PIUArchiveImpl::add(const std::string &path) throw(UndefinedException,
                                                  FileNotFound,
                                                  BadParameter,
                                                  SymlinksNotAllowed){
    boost::filesystem::path p(path.c_str());
    if(boost::filesystem::exists(p)) {
        if(!boost::filesystem::is_symlink(p)) {
            add(p);
        } else {
            throw SymlinksNotAllowed("path argument pointing to symlink passed to PIUArchive::add()");
        }
    } else {
        throw FileNotFound("File \""+path+"\" not found");
    }

}

boost::filesystem::path PIUArchiveImpl::makeRelativePath(boost::filesystem::path base,
                                                         boost::filesystem::path path) {

    boost::filesystem::path::iterator pathIt = path.end();
    boost::filesystem::path::iterator baseIt = base.end();

    bool foundParent = false;

    for(; pathIt != path.begin() && !foundParent; ) {
        pathIt--;
        for(baseIt = base.end() ; baseIt != base.begin() && !foundParent; ) {
            baseIt--;
            if(*pathIt == *baseIt) foundParent = true;
        }
    }
    boost::filesystem::path result;
    for(pathIt++; pathIt != path.end(); pathIt++) {
        boost::filesystem::path dir;
        dir = *pathIt;
        result /= dir;
    }

    return result;
}

bool PIUArchiveImpl::exists(const std::string &fileName) {
    if(this->posMap.find(fileName) == this->posMap.end()) {
        return false;
    } else {
        return true;
    }
}

/** END DRAFT **/
} // end namespace PIU
