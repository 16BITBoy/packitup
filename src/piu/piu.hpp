/*
 * piuarchive.hpp
 *
 *  Created on: 26/05/2012
 *      Author: Adrián Pérez Heredia
 */
#ifndef PIU_HPP_
#define PIU_HPP_
#include <string>
#include <vector>
#include "../fdhandler/fdhandler.hpp"

namespace PIU{
typedef unsigned int FileListSize;
typedef unsigned long int FileSize;
typedef unsigned int FileNameLength;

const unsigned int PIU_SIGNATURE_SIZE = 3;
const std::string PIU_SIGNATURE = "PIU";

class FileInfo{
public:
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

/**
  \brief Exception base class.
**/
class PIUArchiveException : public std::exception{
public:
    const char* what() const throw()
    {
        return "Unknown exception while handling PIU archive.";
    }
};

/**
  \brief Exception: The PIU archive signature is invalid.
**/
class InvalidSignatureException : public PIUArchiveException{
public:
    const char* what() const throw()
    {
        return "PIU archive signature invalid.";
    }
};

/**
  \brief Exception: The PIU archive doesn't exists.
**/
class PIUArchiveDoesNotExists : public PIUArchiveException{
    const char* what() const throw(){
        return "PIU archive doesn't exists.";
    }
};

/**
  \brief Represents a "ToDo list" with the changes to be done in the contents of the PIU archive.
**/
class Operations{
public:
    std::vector<std::string> newFiles; // New files to be written into the new PIU archive.
    std::vector<bool> filesKept; /* Files to be written from the old PIU archive to the new one.
                               The position of every boolean value in the vector corresponds
                               to the position of every file in the old PIU archive.
                            */
};


/**
  \brief Top level class. Provides methods for all the highest level operations related
         to a PIU archive.
**/
class PIUArchive{
private:
    std::string fileName;
    PIUHeader headerInfo;
    std::vector<Data> fileData;
    Operations ops; // Operations to be done in this PIU archive.
    unsigned long int getFileOffset(int position); // Get file offset within PIU archive.
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
} /* End of namespace PIU */

#endif /* _PIU_HPP_ */
