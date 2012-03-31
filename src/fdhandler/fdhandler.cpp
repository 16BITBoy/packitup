#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

#ifdef __unix__
#include <unistd.h>
#endif

#ifdef _WIN32
#include <direct.h>
#endif

#include "fdhandler.hpp"

using namespace std;

DATA::DATA(){
    data = NULL;
    size = 0;
}

DATA::DATA(void *address, unsigned long size){
    data = address;
    this->size = size;
}

DATA::~DATA(){
    char *ptr = (char *) this->data; //cast to char* in order to delete that pointer
    delete[] ptr;
}


FDHANDLER::FDHANDLER(string filepath){
    this->filepath.append(filepath);
    this->_error = false;
}

bool FDHANDLER::error(){
    return this->_error;
}

DATA *FDHANDLER::readall(){
    if(!this->filepath.compare("") || this->filepath.empty()){ /* filepath not specified */
        this->_error = true;
        return NULL;
    }
    
    int ssplitp; /* position where the filepath will be splitted */
    ssplitp = filepath.find_last_of('/');
    string path = filepath.substr(0, ssplitp); /* path where file is located */
    string filename = filepath.substr(ssplitp + 1); /* file name */

    char *wd = NULL;
    /* Get the current working directory */
#ifdef __unix__
    wd = get_current_dir_name();
#endif
#ifdef _WIN32
    wd = _getcwd(NULL, 0);
#endif

    if(path.compare(filename) != 0){ //Returns 0 if true.
    /* change dir to path*/
#ifdef __unix__
        if(chdir(path.c_str()) < 0){
            perror(path.c_str());
            this->_error = true;
            return NULL;
        }
#endif
#ifdef _WIN32
        if(_chdir(path.c_str()) < 0){
            cout << "Unable to access directory '" << path.c_str() << "'" << endl;
            this->_error = true;
            return NULL;
        }
#endif
    }
    
    fstream fs(filename.c_str(), ios::in | ios::binary);
    if(fs.fail()){
        cout << "Error opening file '" << filename << "'" << endl;
        this->_error = true;
        return NULL;
    }
        
    /* get filesize */
    fs.seekg(0, ios::end);
    unsigned long filesize = fs.tellg();
    
    /* reset offset to 0 */
    fs.seekg(0, ios::beg);
    
    /* allocate bytes needed */
    DATA *filedata = new DATA();
    filedata->data = new char[filesize];
    
    /* read file and that's all folks :D */
    fs.read((char*)filedata->data, filesize);
    filedata->size = fs.gcount();
    fs.close();
#ifdef __unix__
    chdir(wd);
#endif
#ifdef _WIN32
    _chdir(wd);
#endif
    return filedata;
}

DATA *FDHANDLER::readchk(unsigned long offset, unsigned long size){
    if(!this->filepath.compare("") || this->filepath.empty()){
        this->_error = true;
        return NULL;
    }
    
    int ssplitp; /* position where the filepath will be splitted */

	//folder separator changes from unix to windows
#ifdef __unix__
    char separator = '/';
#endif
#ifdef _WIN32
    char separator = '\\';
#endif
    ssplitp = filepath.find_last_of(separator); //split where last folder separator is located
    string path = filepath.substr(0, ssplitp); /* path where file is located */
    string filename = filepath.substr(ssplitp + 1); /* file name */

    char *wd = NULL;
    /* Get the current working directory */
#ifdef __unix__
    wd = get_current_dir_name();
#endif
#ifdef _WIN32
    wd = _getcwd(NULL, 0);
#endif

    /* Since path and filename are the same when there is
         * no directory in filepath, we check for this equality
         * to prevent changing current directory.
         */
    if(path.compare(filename) != 0){ //Returns 0 if true.
    /* change dir to path*/
#ifdef __unix__
        if(chdir(path.c_str()) < 0){
            perror(path.c_str());
            this->_error = true;
            return NULL;
        }
#endif
#ifdef _WIN32
        if(_chdir(path.c_str()) < 0){
            cout << "Unable to access directory '" << path.c_str() << "'" << endl;
            this->_error = true;
            return NULL;
        }
#endif
    }
    
    fstream fs(filename.c_str(), ios::in | ios::binary);
    if(fs.fail()){
        cout << "Error opening file '" << filename << "'" << endl;
        this->_error = true;
        return NULL;
    }
        
    /* get filesize */
    fs.seekg(0, ios::end);
    unsigned long filesize = fs.tellg();
    
    /* set offset to 'offset' */
    fs.seekg(offset, ios::beg);
    
    /* allocate bytes needed */
    DATA *filedata = new DATA();
    filedata->data = new char[size];
    
    fs.read((char*)filedata->data, size);
    filedata->size = fs.gcount();
    fs.close();
#ifdef __unix__
    chdir(wd);
#endif
#ifdef _WIN32
    _chdir(wd);
#endif
    /* If the read operation failed retrieving 'size' bytes, return bytes read */
    if(fs.fail()){
        this->_error = true;
        return filedata;
    }
    return filedata;
}

FDHANDLER& FDHANDLER::write(DATA *data){
    if(!this->filepath.compare("") || this->filepath.empty()){
        this->_error = true;
        return *this;
    }

    int ssplitp; /* position where the filepath will be splitted */

    //folder separator changes from unix to windows
#ifdef __unix__
    char separator = '/';
#endif
#ifdef _WIN32
    char separator = '\\';
#endif
    ssplitp = filepath.find_last_of(separator); //split where last folder separator is located
    string path = filepath.substr(0, ssplitp); /* path where file is located */
    string filename = filepath.substr(ssplitp + 1); /* file name */

    char *wd = NULL;
    /* Get the current working directory */
#ifdef __unix__
    wd = get_current_dir_name();
#endif
#ifdef _WIN32
    wd = _getcwd(NULL, 0);
#endif

    /* Since path and filename are the same when there is
         * no directory in filepath, we check for this equality
         * to prevent changing current directory.
         */
    if(path.compare(filename) != 0){ //Returns 0 if true.
    /* change dir to path*/
#ifdef __unix__
        if(chdir(path.c_str()) < 0){
            perror(path.c_str());
            this->_error = true;
            return *this;
        }
#endif
#ifdef _WIN32
        if(_chdir(path.c_str()) < 0){
            cout << "Unable to access directory '" << path.c_str() << "'" << endl;
            this->_error = true;
            return *this;
        }
#endif
    }

    fstream fs(filename.c_str(), ios::out | ios::binary);
    if(fs.fail()){
        cout << "Error opening file '" << filename << "'" << endl;
        this->_error = true;
        return *this;
    }

    fs.write((char*)data->data, data->size);
    if(fs.bad()){
        this->_error = true;
        return *this;
    }

    fs.close();

#ifdef __unix__
    chdir(wd);
#endif
#ifdef _WIN32
    _chdir(wd);
#endif

    free(wd);
    return *this;
}

bool FDHANDLER::operator!(){
    return this->error();
}
