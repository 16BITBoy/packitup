#ifndef FILEFORMAT_HPP
#define FILEFORMAT_HPP
#include <vector>
#include <string>
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
#endif // FILEFORMAT_HPP
