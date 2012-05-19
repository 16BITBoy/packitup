#ifndef _FDHandler_HPP
#define _FDHandler_HPP

/**
 * \brief The Data class represents a chunk of data retrieved from a file.
 * 
 * \details The Data class consists in a pointer of no type (void type)
 * and a size field of type unsigned long, with the size of the chunk in
 * bytes.
 */
class Data {
public:
    void *data; /* place where data is located */
    unsigned long size; /* size of data */
    Data();
    Data(void *address, unsigned long size);
    void free();
    ~Data();
};

/**
 * \brief This class allows fast coding of file data reading and writing.
 * Works using Data objects to return the data retrieved and to write data
 * to a file.
 */
class FDHandler {
    public:
    std::string filepath;
    
    FDHandler(std::string filepath);
    
    /**
     * \brief Loads the entire file and stores its data in the Data structure given at filedata.
     *
     * \details First attepts to read the whole file specified in FDHandler::filepath. If it
     * succeded, stores its data in the Data object given at filedata.
     * In case an error happened, FDHandler::error() returns true.
     * \return The instance of the object that called its method.
     * */
    FDHandler &readall(Data *filedata);
    
    /**
     * \brief Loads only a chunk of the file of 'size' bytes from
     * the position at 'offset'.
     *
     * \details First attepts to read the whole file specified in FDHandler::filepath. If it
     * succeded, stores the read data into the Data object given at 'filedata'.
     * In case an error happened, FDHandler::error() returns true.
     * \return The instance of the object that called its method.
     * */
    FDHandler &readchk(Data *filedata, unsigned long offset, unsigned long size);
    
    /**
     * \brief Writes the data stored in the object given at 'data' into the file specified
     * in FDHandler::filepath.
     *
     * \details Writes the data stored in the object given at 'data' into the file specified
     * in FDHandler::filepath. In case an error happened, FDHandler::error() returns true.
     * \return The instance of the object that called its method.
     * */
    FDHandler &write(Data *data);
    
    /**
     * \brief Writes the data stored in the object given at 'data' into the end of the file 
     * specified in FDHandler::filepath.
     *
     * \details Writes the data stored in the object given at 'data' into the end of the file 
     * specified in FDHandler::filepath. In case an error happened, FDHandler::error()
     * returns true.
     * \return The instance of the object that called its method.
     * */
    FDHandler &append(Data *data);
    
    FDHandler &write(char *data, unsigned long size);
    FDHandler &append(char *data, unsigned long size);

    /**
     * \brief Tells if the object is in error state.
     *
     * \details A FDHandler object is in error state if a problem happened during the execution
     * of one of its file IO methods. For instance, if a problem happened because it could not find
     * the file required for a read or write operation, this method would return true.
     * \return False if the object is new, or none of the operations done has failed. True if some
     * of the operations done has failed.
     * */
    bool error();
    
    /**
     * \brief Operator ! overload.
     *
     * \details Works the same way as the FDHandler::error() method, so the expression:
     * "if(!object)" will be evaluated like "if(object->error())".
     * \return False if the object is new, or none of the operations done has failed. True if some
     * of the operations done has failed.
     * */
    bool operator!();

    private:
    bool _error;
    void resetcwd(char *wd);
    void _write(char *data, unsigned long size, bool append);
};
#endif //_FDHandler_HPP
