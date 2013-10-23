/*
 * piuarchive.hpp
 *
 *  Created on: 26/05/2012
 *      Author: Adrián Pérez Heredia
 */
#ifndef PIU_IMPL_HPP_
#define PIU_IMPL_HPP_

#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include "exceptions.hpp"
#include "operations.hpp"
#include "fileformat.hpp"

namespace PIU{

class PIUArchiveImpl{
private:
    std::string fileName;
    PIUHeader headerInfo;

    Operations ops; // Operations to be done in this PIU archive.
    boost::unordered::unordered_map<std::string, FileListSize> posMap; // Maps filenames with the position
                                                              // within the PIU archive.
    unsigned long int getFileOffset(int position); // Get file offset within PIU archive.
    void getHeaderInfo() throw(UndefinedException,
                               FileFormatError); // Gets header info from file on disk.
    void computeFileListSize();
    void add(boost::filesystem::path path) throw(UndefinedException,
                                                 FileNotFound);

    /**
     * \brief Adds a new file to the archive in memory (just adds the file information to the
     *        file header in memory)
     * @param fileName Name of the file to add.
     * @pre   fileName must not be empty. If so, BadParameter exception is thrown.
     * @post  If there is no problems during the read of the file, its information will be stored
     *        in memory for later be writen on disk when write() is called.
     **/
    void addFile(boost::filesystem::path &file) throw(UndefinedException,
                                                      FileNotFound);
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
    void addDirectory(boost::filesystem::path &dirPath) throw(UndefinedException,
                                                              FileNotFound);

    /**
     * TODO: Very drafty spec. Review it.
     * @brief adds a new path to the archive and process the file if it points to a single file by adding it into the
     *        archive in memory, or every file found in the path if it points to a directory.
     *        (just adds the information of each element to the file header in memory)
     * @param path it can point to a directory or a file
     * @pre   path must not be empty. If so, BadParameter exception is thrown.
     * @post  If the path doesn't exists, FileNotFound exception is thrown.
     *        Otherwise it stores every file, including subdirectories from the directory specified.
     *        if there is a problem with a particular file, either FileNotFound or UnexpectedException is thrown when
     *        apropiate,
     */

    boost::filesystem::path makeRelativePath(boost::filesystem::path base,
                                             boost::filesystem::path path);
public:
    /**
     * \brief Specifies the file to work with and if the file exists loads the header information.
     * @param fileName Name of the file to load, or to be created in case it doesn't exists.
     * @pre   fileName string must not be empty.
     * @post  If the file exist, loads the header information of the file. If it doesn't exists,
     *        this object remains empty and ready for use as a new PIU file.
     **/
    PIUArchiveImpl(std::string &fileName) throw(UndefinedException,
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

    void add(const std::string &path) throw(UndefinedException,
                                     FileNotFound,
                                     BadParameter,
                                     SymlinksNotAllowed);

    /** \brief Deletes a file from the archive in memory (just deletes the header information to the file in memory) **/
    void deleteFile(const std::string &fileName);
    /** \brief Reads the data from the file specified in the archive in memory and writes it into the specified path **/
    void extractFile(const std::string &fileName) throw (FileNotFound);
    void updateHeaderInfo() throw(UndefinedException,
                                  FileFormatError);
    bool exists(const std::string &fileName);
};
} /* End of namespace PIU */
/** END DRAFT **/
#endif /* _PIU_IMPL_HPP_ */
