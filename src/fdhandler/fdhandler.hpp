#ifndef _FDHANDLER_HPP
#define _FDHANDLER_HPP

class DATA {
    public:
    void *data; /* place where data is located */
    unsigned long size; /* size of data */
    DATA();
    DATA(void *address, unsigned long size);
    ~DATA();
};

class FDHANDLER {
    public:
    std::string filepath;
    
    FDHANDLER(std::string filepath);
    
    /* load the entire file and return a DATA structure
     * with the file data */
    DATA *readall();
    
    /* load only a chunk of the file of 'size' bytes from
     * the position at 'offset' */
    DATA *readchk(unsigned long offset, unsigned long size);
    
    /* save data 'data' into the current file, overwriting it. */
    FDHANDLER& write(DATA *data);
    
    /* save data 'data' at the end of the current file */
    unsigned long append(DATA *data);
    
    /* true if an error happened during a call to any of the read
     * and write methods.
     */
    bool error();

    /* close the current file */
    void close();

    private:
    bool _error;
};
#endif //_FDHANDLER_HPP
