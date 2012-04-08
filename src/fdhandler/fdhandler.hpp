#ifndef _FDHANDLER_HPP
#define _FDHANDLER_HPP

/**
 * \brief The DATA class represents a chunk of data retrieved from a file.
 * 
 * \details The DATA class consists in a pointer of no type (void type) 
 * and a size field of type unsigned long, with the size of the chunk in
 * bytes.
 */
class DATA {
    public:
    void *data; /* place where data is located */
    unsigned long size; /* size of data */
    DATA();
    DATA(void *address, unsigned long size);
    ~DATA();
};

/**
 * \brief This class allows fast coding of file data reading and writing.
 * Works using DATA objects to return the data retrieved and to write data
 * to a file.
 */
class FDHANDLER {
    public:
    std::string filepath;
    
    FDHANDLER(std::string filepath);
    
    /**
     * \brief Loads the entire file and stores its data in the DATA structure given at filedata.
     *
     * \details First attepts to read the whole file specified in FDHANDLER::filepath. If it
     * succeded, stores its data in the DATA object given at filedata.
     * In case an error happened, FDHANDLER::error() returns true.
     * \return The instance of the object that called its method.
     * */
    FDHANDLER &readall(DATA *filedata);
    
    /**
     * \brief Loads only a chunk of the file of 'size' bytes from
     * the position at 'offset'.
     *
     * \details First attepts to read the whole file specified in FDHANDLER::filepath. If it
     * succeded, stores the read data into the DATA object given at 'filedata'.
     * In case an error happened, FDHANDLER::error() returns true.
     * \return The instance of the object that called its method.
     * */
    FDHANDLER &readchk(DATA *filedata, unsigned long offset, unsigned long size);
    
    /**
     * \brief Writes the data stored in the object given at 'data' into the file specified
     * in FDHANDLER::filepath.
     *
     * \details Writes the data stored in the object given at 'data' into the file specified
     * in FDHANDLER::filepath. In case an error happened, FDHANDLER::error() returns true.
     * \return The instance of the object that called its method.
     * */
    FDHANDLER &write(DATA *data);
    
    /**
     * \brief Writes the data stored in the object given at 'data' into the end of the file 
     * specified in FDHANDLER::filepath.
     *
     * \details Writes the data stored in the object given at 'data' into the end of the file 
     * specified in FDHANDLER::filepath. In case an error happened, FDHANDLER::error() 
     * returns true.
     * \return The instance of the object that called its method.
     * */
    unsigned long append(DATA *data);
    
    /**
     * \brief Tells if the object is in error state.
     *
     * \details A FDHANDLER object is in error state if a problem happened during the execution
     * of one of its file IO methods. For instance, if a problem happened because it could not find
     * the file required for a read or write operation, this method would return true.
     * \return False if the object is new, or none of the operations done has failed. True if some
     * of the operations done has failed.
     * */
    bool error();
    
    /**
     * \brief Operator ! overload.
     *
     * \details Works the same way as the FDHANDLER::error() method, so the expression:
     * "if(!object)" will be evaluated like "if(object->error())".
     * \return False if the object is new, or none of the operations done has failed. True if some
     * of the operations done has failed.
     * */
    bool operator!();

    private:
    bool _error;
    void resetcwd(char *wd);
};
#endif //_FDHANDLER_HPP
