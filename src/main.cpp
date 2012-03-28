#include <iostream>
#include <cassert>
#include <string>

using namespace std;

class DATA {
    public:
    void *data; /* place where data is located */
    unsigned long size; /* size of data */
    DATA();
    DATA(void *address, unsigned long size);
    ~DATA();
};
    
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

#include <fstream>
#ifdef __unix__
#include <unistd.h>
#endif
#ifdef _WIN32
#include <direct.h>
#endif
class FDHANDLER {
    public:
    string filepath;
    
    FDHANDLER(string filepath);
    
    /* load the entire file and return a DATA structure
     * with the file data */
    DATA *readall();
    
    /* load only a chunk of the fie of 'size' bytes from
     * the position at 'offset' */
    DATA *readchk(unsigned long offset, unsigned long size);
    
    /* save data 'data' into the current file, overwriting it. */
    DATA *save(DATA *data);
    
    /* save data 'data' at the end of the current file */
    DATA *append(DATA *data);
    
    /* close the current file */
    void close();
   
};

FDHANDLER::FDHANDLER(string filepath){
    this->filepath.append(filepath);
}

DATA *FDHANDLER::readall(){
    if(!this->filepath.compare("") || this->filepath.empty()){ /* filepath not specified */
        return NULL;
    }
    
    int ssplitp; /* position where the filepath will be splitted */
    ssplitp = filepath.find_last_of('/');
    string path = filepath.substr(0, ssplitp); /* path where file is located */
    string filename = filepath.substr(ssplitp + 1); /* file name */

	/* get the current working dir and change it to 'path' */
#ifdef __unix__
    char *wd = get_current_dir_name(); /* current working dir */

    if(chdir(path.c_str()) < 0){
        perror(path.c_str());
		return NULL;
    }    
#endif
#ifdef _WIN32
	char *wd = NULL;
	_getcwd(wd, 32767); /* the size passed as second parameter is the max path characters in
						   windows */
	if(_chdir(path.c_str()) < 0){
		cout << "Unable to access directory '" << path.c_str() << "'" << endl;
		return NULL;
	}
#endif

    
    fstream fs(filename.c_str(), ios::in | ios::binary);
    if(fs.fail()){
        cout << "Error opening file '" << filename << "'" << endl; 
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
    if(!this->filepath.compare("") || this->filepath.empty())
        return NULL;
    
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
    
	/* get the current working dir and change it to 'path' */
#ifdef __unix__
    char *wd = get_current_dir_name(); /* current working dir */

    if(chdir(path.c_str()) < 0){
        perror(path.c_str());
		return NULL;
    }    
#endif
#ifdef _WIN32
	char *wd = NULL;
	wd = _getcwd(NULL, 0); /* the size passed as second parameter is the max path characters in
						   windows */
	if(_chdir(path.c_str()) < 0){
		cout << "Unable to access directory '" << path.c_str() << "'" << endl;
		return NULL;
	}
#endif
    
    fstream fs(filename.c_str(), ios::in | ios::binary);
    if(fs.fail()){
        cout << "Error opening file '" << filename << "'" << endl; 
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
    return filedata;
}

void test_data(){
    DATA *data = new DATA();
    assert(data != NULL);
    assert(data->data == NULL);
    assert(data->size == 0);
    delete data;
    
    int i = 453;
    data = new DATA(&i, sizeof i);
    assert(data != NULL);
    assert(data->data == &i);
    assert(data->size == sizeof i);
    delete data;
    
    int *a = new int[3];
    data = new DATA(a, sizeof(int)*3);
    assert(data != NULL);
    assert(data->data == a);
    assert(data->size == sizeof(int)*3);
    delete data;
    cout << "Tests passed for DATA class." << endl;
}

/* Manual test for readall function */
void test_fdhandler_readall(string file){
    FDHANDLER fd(file);
    DATA *filedata = fd.readall();
    assert(filedata != NULL);
    assert(filedata->data != NULL);
    assert(filedata->size > 0);
    cout << filedata->size << " bytes read" << endl;
    fstream fs("copy", ios::out | ios::binary);
    fs.write((char*)filedata->data, filedata->size);
    fs.close();
    cout << "File copied into 'copy'. Check content." << endl;
    delete filedata;
}

/* Manual test for readall function */
void test_fdhandler_readchk(string file, unsigned long offset, unsigned long size){
    FDHANDLER fd(file);
    DATA *filedata = fd.readchk(offset, size);
    assert(filedata != NULL);
    assert(filedata->data != NULL);
    assert(filedata->size == size);
    cout << filedata->size << " bytes read" << endl;
    fstream fs("copy", ios::out | ios::binary);
    fs.write((char*)filedata->data, filedata->size);
    fs.close();
    cout << "file copied into 'copy'" << endl;
    delete filedata;
}

int main(int argc, char **argv){
	char c;
	/*fstream fs("fichero", ios::out | ios::binary);
	char *data = new char[1024 * 1024];
	unsigned long i;
	for(i = 0; i < 1024 * 1024; i++){
		data[i] = 1;
	}
	fs << data;*/
    test_fdhandler_readchk("carpeta\\fichero", 7, 5);
	cin >> c;
    return 0;
}
