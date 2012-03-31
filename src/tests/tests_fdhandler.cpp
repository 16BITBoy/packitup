#include <iostream>
#include <cassert>
#include <fstream>

#include "../fdhandler/fdhandler.hpp"

using namespace std;

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

/* Manual test for write function */
void test_fdhandler_write(string outfile){
    FDHANDLER fd(outfile);
    DATA *buffer = new DATA(new char[1024], 1024);
    unsigned long i;
    char *c;
    for(i = 0; i < 1024; i++){
        c = (char*)buffer->data;
        *(c + i) = 7;
    }
    fd.write(buffer);
    assert(!fd.error());
    delete buffer;
}

