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

namespace PIU{

/** BEGIN DRAFT **/
void PIUArchive::updateHeaderInfo() throw(UndefinedException,
                                          FileFormatError){
    this->impl->updateHeaderInfo();
}

PIUArchive::PIUArchive(std::string &fileName) throw (UndefinedException,
                                                    BadParameter){
    this->impl = new PIUArchiveImpl(fileName);
}

/* TODO: Calling this function will require to update all the header information from the new
         on disk version
 */
void PIUArchive::write() throw(UndefinedException,
                               FileNotFound){
    this->impl->write();
}

/* TODO: Calling this function will require to recalculate this->headerInfo.fileListSize after
   the call to keep it updated. */
void PIUArchive::deleteFile(const std::string &fileName){
    this->impl->deleteFile(fileName);
}

std::vector<FileInfo> PIUArchive::listFiles(){
    return this->impl->listFiles();
}

FileListSize PIUArchive::getFileListSize(){
    return this->impl->getFileListSize();
}

void PIUArchive::add(std::string &path) throw(UndefinedException,
                                             FileNotFound,
                                             BadParameter,
                                             SymlinksNotAllowed){
    this->impl->add(path);
}

bool PIUArchive::exists(std::string &fileName) {
    this->impl->exists(fileName);
}

void PIUArchive::extractFile(const std::string &fileName) throw (FileNotFound) {
    this->impl->extractFile(fileName);
}

/** END DRAFT **/
} // end namespace PIU
