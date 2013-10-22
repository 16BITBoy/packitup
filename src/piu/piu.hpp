/*
 * piuarchive.hpp
 *
 *  Created on: 26/05/2012
 *      Author: Adrián Pérez Heredia
 */
#ifndef PIU_HPP_
#define PIU_HPP_
#include <vector>
#include <string>
#include "exceptions.hpp"
#include "fileformat.hpp"

namespace PIU{

/** BEGIN DRAFT **/

/**
  \brief Top level class. Provides methods for all the highest level operations related
         to a PIU archive.
**/
class PIUArchiveImpl;
class PIUArchive{
private:
    PIUArchiveImpl *impl;
public:
    /**
     * \brief Specifies the file to work with and if the file exists loads the header information.
     * @param fileName Name of the file to load, or to be created in case it doesn't exists.
     * @pre   fileName string must not be empty.
     * @post  If the file exist, loads the header information of the file. If it doesn't exists,
     *        this object remains empty and ready for use as a new PIU file.
     **/
    PIUArchive(std::string &fileName) throw(UndefinedException,
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

    void add(std::string &path) throw(UndefinedException,
                                     FileNotFound,
                                     BadParameter,
                                     SymlinksNotAllowed);

    /** \brief Deletes a file from the archive in memory (just deletes the header information to the file in memory) **/
    void deleteFile(const std::string &fileName);
    /** \brief Reads the data from the file specified in the archive in memory and writes it into the specified path **/
    void extractFile(const std::string &fileName, const std::string &extractPath);
    void updateHeaderInfo() throw(UndefinedException,
                                  FileFormatError);
    bool exists(std::string &fileName);
};
} /* End of namespace PIU */
/** END DRAFT **/
#endif /* _PIU_HPP_ */
