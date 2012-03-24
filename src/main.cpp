#include <iostream>
#include <fstream>
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
    char *ptr = (char *) this->data;
    delete ptr;
}

class FDHANDLER {
    public:
    string filepath;
    
    FDHANDLER(string filepath);
    
    /* load the entire file and return a DATA structure
     * with the file data */
    DATA *readall();
    
    /* load only a chunk of the fie of 'size' bytes from
     * the position at 'offset' */
    DATA *readchk(unsigned long size, unsigned long offset);
    
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
    
    fstream fs(this->filepath.c_str(), ios::in | ios::binary);
    
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

int main(int argc, char **argv){
    FDHANDLER fd("out");
    DATA *filedata = fd.readall();
    assert(filedata != NULL);
    assert(filedata->data != NULL);
    assert(filedata->size > 0);
    cout << filedata->size << " bytes read" << endl;
    fstream fs("copy", ios::out | ios::binary);
    fs.write((char*)filedata->data, filedata->size);
    fs.close();
    cout << "file copied into 'copy'" << endl;
    return 0;
}
