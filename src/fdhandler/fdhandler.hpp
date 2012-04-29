#ifndef _FDHANDLER_HPP
#define _FDHANDLER_HPP

class DATA {
public:
    void *data; /* place where data is located */
    unsigned long size; /* size of data */
    DATA(bool dynamic);
    DATA(void *address, unsigned long size, bool dynamic);
    ~DATA();

private:
    bool isDynAlloc;

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
     * \brief Loads the entire file and returns a pointer to DATA object
     * with the file data.
     *
     * \details First attepts to read the whole file specified in FDHANDLER::filepath. If it
     * succeded, returns a pointer to DATA object with all the file data.
     * In case an error happened, returns a NULL pointer and if FDHANDLER::error() is called
     * after, returns true.
     * */
    FDHANDLER &readall(DATA *filedata);
    
    /* load only a chunk of the file of 'size' bytes from
     * the position at 'offset' */
    FDHANDLER &readchk(DATA *filedata, unsigned long offset, unsigned long size);
    
    /* save data 'data' into the current file, overwriting it. */
    FDHANDLER &write(DATA *data);
    
    /* save data 'data' at the end of the current file */
    unsigned long append(DATA *data);
    
    /* true if an error happened during a call to any of the read
     * and write methods.
     */
    bool error();

    bool operator!();

    private:
    bool _error;
    void resetcwd(char *wd);
};
#endif //_FDHANDLER_HPP
