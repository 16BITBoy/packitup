/*
 * piuarchive.hpp
 *
 *  Created on: 26/05/2012
 *      Author: Adrián Pérez Heredia
 */
#ifndef PIU_HPP_
#define PIU_HPP_
#include <boost/unordered_map.hpp>
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
    std::string filePath;
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
class UndefinedException : public std::runtime_error{
public:
    UndefinedException(std::string msg) :
        std::runtime_error("Undefined PackItUp exception: " + msg) {}
};


/**
  \brief Exception: The PIU archive signature is invalid.
**/
class FileFormatError : public std::runtime_error{
public:
    FileFormatError(std::string msg) :
        std::runtime_error("This is not a valid PIUArchive file: " + msg){}

};

/**
  \brief Exception: PIUArchive constructor called with empty string
**/
class BadParameter : public std::runtime_error{
public:
    BadParameter(std::string msg) :
        std::runtime_error("Bad parameter value error: " + msg){}
};

/**
  \brief Exception: The PIU archive doesn't exists.
**/
class FileNotFound : public std::runtime_error{
public:
    FileNotFound(std::string msg)
        : std::runtime_error("File not found exception: " + msg){}

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

/** BEGIN DRAFT **/

/**
  \brief Top level class. Provides methods for all the highest level operations related
         to a PIU archive.
**/
class PIUArchive{
private:
    std::string fileName;
    PIUHeader headerInfo;
    // std::vector<Data> fileData; //Not sure if it will get ever used...
    Operations ops; // Operations to be done in this PIU archive.
    boost::unordered::unordered_map<std::string, FileListSize> posMap; // Maps filenames with the position
                                                              // within the PIU archive.
    unsigned long int getFileOffset(int position); // Get file offset within PIU archive.
    void getHeaderInfo() throw(UndefinedException,
                               FileFormatError); // Gets header info from file on disk.
    void computeFileListSize();
public:
    /**
     * \brief Specifies the file to work with and if the file exists loads the header information.
     * @param fileName Name of the file to load, or to be created in case it doesn't exists.
     * @pre   fileName string must not be empty.
     * @post  If the file exist, loads the header information of the file. If it doesn't exists,
     *        this object remains empty and ready for use as a new PIU file.
     **/
    PIUArchive(std::string fileName) throw(UndefinedException,
                                           BadParameter);
    /**
     * \brief Returns a vector with the information about the files in the archive
     * @post  Returns std::vector<FileInfo> with all the information about the files, if there are any.
     *        Vector is empty otherwise.
     **/
    std::vector<FileInfo> listFiles();

    /**
     * @brief  Returns the size in bytes of the filelist.
     * @return Returns a FileListSize value.
     **/
    FileListSize getFileListSize();

    /**
     * \brief Dumps changes to the file on disk.
     * @pre   fileName should be a valid one. If not, behaviour is not defined.
     * @post  Writes the changes in a new temporary file, copies data from the old PIU file if there
     *        was any, then deletes that old file and renames the temporary file to the name specified
     *        by fileName member.
     **/
    void write() throw(UndefinedException,
                       FileNotFound);
    /**
     * \brief Adds a new file to the archive in memory (just adds the file information to the
     *        file header in memory)
     * @param fileName Name of the file to add.
     * @pre   fileName must not be empty. If so, BadParameter exception is thrown.
     * @post  If there is no problems during the read of the file, its information will be stored
     *        in memory for later be writen on disk when write() is called.
     **/
    void addFile(std::string fileName) throw(UndefinedException,
                                             FileNotFound,
                                             BadParameter);
    /**
     * TODO: Very drafty spec. Review it.
     * @brief addDirectory adds a new directory with all its files and subdirectories in it, into the archive in memory
     *        (just adds the information of each element to the file header in memory)
     * @param dirPath Path to the directory
     * @pre   dirPath must not be empty. If so, BadParameter exception is thrown.
     * @post  If the directory doesn't exists, FileNotFound exception is thrown.
     *        otherwise it stores every file, including subdirectories from the directory specified.
     *        if there is a problem with a particular file, either FileNotFound or PIUArchiveException is thrown when
     *        apropiate,
     */
    void addDirectory(std::string dirPath) throw(UndefinedException,
                                                 FileNotFound,
                                                 BadParameter);

    /** \brief Deletes a file from the archive in memory (just deletes the header information to the file in memory) **/
    void deleteFile(std::string fileName);
    /** \brief Reads the data from the file specified in the archive in memory and writes it into the specified path **/
    void extractFile(std::string fileName, std::string extractPath);
    void updateHeaderInfo() throw(UndefinedException,
                                  FileFormatError);
};
} /* End of namespace PIU */
/** END DRAFT **/
#endif /* _PIU_HPP_ */
