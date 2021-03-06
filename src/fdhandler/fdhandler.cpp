#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

#if defined (__unix__) || (__APPLE__)
#include <unistd.h>
#endif

#ifdef _WIN32
#include <direct.h>
#endif

#include "fdhandler.hpp"

using namespace std;

Data::Data(){
    data = NULL;
    size = 0;
}

Data::Data(void *address, unsigned long size){
    data = address;
    this->size = size;
}

Data::~Data(){
    this->free();
}

void Data::free(){
    size = 0;
    char *ptr = (char *) this->data; //cast to char* in order to delete that pointer
    delete[] ptr;
}

FDHandler::FDHandler(string filepath){
    this->filepath.append(filepath);
    this->_error = false;
}

bool FDHandler::error(){
    return this->_error;
}

FDHandler &FDHandler::readall(Data *filedata){
    if(!this->filepath.compare("") || this->filepath.empty()){ /* filepath not specified */
        this->_error = true;
        return *this;
    }
    
    int ssplitp; /* position where the filepath will be splitted */
    ssplitp = filepath.find_last_of('/');
    string path = filepath.substr(0, ssplitp); /* path where file is located */
    string filename = filepath.substr(ssplitp + 1); /* file name */

    char *wd = NULL;
    /* Get the current working directory */
#if defined (__unix__) || (__APPLE__)
    wd = getcwd(NULL,0);
#endif
#ifdef _WIN32
    wd = _getcwd(NULL, 0);
#endif

    if(path.compare(filename) != 0){ //Returns 0 if true.
    /* change dir to path*/
#if defined (__APPLE__) || (__unix__)
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
    
    fstream fs(filename.c_str(), ios::in | ios::binary);
    if(fs.fail()){
        cout << "Error opening file '" << filename << "'" << endl;
        this->_error = true;
        return *this;
    }
        
    /* get filesize */
    fs.seekg(0, ios::end);
    unsigned long filesize = fs.tellg();
    
    /* reset offset to 0 */
    fs.seekg(0, ios::beg);
    
    /* allocate bytes needed */
    if(filedata == NULL){ /* 'filedata' MUST be allocated before call this method */
        this->_error = true;
        return *this;
    }
    filedata->data = new char[filesize];
    
    /* read file and that's all folks :D */
    fs.read((char*)filedata->data, filesize);
    filedata->size = fs.gcount();
    fs.close();
#if defined (__APPLE__) || (__unix__)
    chdir(wd);
#endif
#ifdef _WIN32
    _chdir(wd);
#endif
    return *this;
}

FDHandler &FDHandler::readchk(Data *filedata, unsigned long offset, unsigned long size){
    if(!this->filepath.compare("") || this->filepath.empty()){
        this->_error = true;
        return *this;
    }

    int ssplitp; /* position where the filepath will be splitted */

	//folder separator changes from unix to windows
#if defined (__APPLE__) || (__unix__)
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
#if defined (__APPLE__) || (__unix__)
    wd = getcwd(NULL, 0);
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
#if defined (__APPLE__) || (__unix__)
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

    fstream fs(filename.c_str(), ios::in | ios::binary);
    if(fs.fail()){
        cout << "Error opening file '" << filename << "'" << endl;
        this->_error = true;
        return *this;
    }

    /* get filesize */
    fs.seekg(0, ios::end);
    unsigned long filesize = fs.tellg();

    /* set offset to 'offset' */
    fs.seekg(offset, ios::beg);

    /* allocate bytes needed */
    filedata->data = new char[size];

    fs.read((char*)filedata->data, size);
    filedata->size = fs.gcount();
    fs.close();
#if defined (__APPLE__) || (__unix__)
    chdir(wd);
#endif
#ifdef _WIN32
    _chdir(wd);
#endif
    /* If the read operation failed retrieving 'size' bytes, return bytes read */
    if(fs.fail()){
        this->_error = true;
    }
    return *this;
}

FDHandler& FDHandler::write(Data *data){
    _write((char *)data->data, data->size, false);
    return *this;
}

FDHandler& FDHandler::append(Data *data){
    _write((char *)data->data, data->size, true);
    return *this;
}

FDHandler &FDHandler::write(char *data, unsigned long size){
    _write(data, size, false);
    return *this;
}

FDHandler &FDHandler::append(char *data, unsigned long size){
    _write(data, size, true);
    return *this;
}

bool FDHandler::operator!(){
    return this->error();
}

void FDHandler::_write(char *data, unsigned long size, bool append){
    if(!this->filepath.compare("") || this->filepath.empty()){
        this->_error = true;
        return;
    }

    int ssplitp; /* position where the filepath will be splitted */

    //folder separator changes from unix to windows
#if defined (__APPLE__) || (__unix__)
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
#if defined (__APPLE__) || (__unix__)
    wd = getcwd(NULL, 0);
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
#if defined (__APPLE__) || (__unix__)
        if(chdir(path.c_str()) < 0){
            perror(path.c_str());
            this->_error = true;
            return;
        }
#endif
#ifdef _WIN32
        if(_chdir(path.c_str()) < 0){
            cout << "Unable to access directory '" << path.c_str() << "'" << endl;
            this->_error = true;
            return;
        }
#endif
    }

    ios_base::openmode omode;
    omode = ios::out | ios::binary;
    if(append){
        omode |= ios::app;
    }
    fstream fs(filename.c_str(), omode);
    if(fs.fail()){
        cout << "Error opening file '" << filename << "'" << endl;
        this->_error = true;
        return;
    }

    fs.write(data, size);
    if(fs.bad()){
        this->_error = true;
        return;
    }

    fs.close();

#if defined (__APPLE__) || (__unix__)
    chdir(wd);
#endif
#ifdef _WIN32
    _chdir(wd);
#endif

    free(wd);
}

