#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP
#include <stdexcept>
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

class SymlinksNotAllowed : public std::runtime_error{
public:
    SymlinksNotAllowed(std::string msg)
        : std::runtime_error("Symlinks are not allowed: " + msg ){}
};
#endif // EXCEPTIONS_HPP
