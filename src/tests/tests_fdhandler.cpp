#include <iostream>
#include <cassert>
#include <fstream>

#include "../fdhandler/fdhandler.hpp"

using namespace std;

/* Manual test for readall function */
void test_fdhandler_readall(string file){ //Needs an actual file to test
    FDHANDLER fd(file);
    DATA *filedata = new DATA(true);
    fd.readall(filedata);
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
void test_fdhandler_readchk(string file, unsigned long offset, unsigned long size){//Needs actual file to test
    FDHANDLER fd(file);
    DATA *filedata = new DATA(true);
    fd.readchk(filedata, offset, size);
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
void test_fdhandler_write(string outfile){//output filename
    FDHANDLER fd(outfile);
    DATA *buffer = new DATA(new char[1024], 1024,true);
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

void test_fdhandler_append(string targetfile){
    cout << "Testing FDHANDLER::append()" << endl << endl;
    FDHANDLER *fd = new FDHANDLER(targetfile);
    string s = "Test passed: ";
    fd->write(new DATA((void *)s.c_str(),13,false));
    fd->append(new DATA((void *)"OK",2,false));
    DATA *result = new DATA(true);
    fd->readall(result);
    string r = (char *)result->data ;
    assert(!r.compare("Test passed: OK"));
    cout << r << endl;
    delete fd;
    delete result;
}

void test_fdhandler__write(string targetfile){
    cout << "Testing FDHANDLER::_write()" << endl << endl;
    FDHANDLER *fd = new FDHANDLER(targetfile);
    string s = "Test passed: ";
    fd->_write((char *)s.c_str(),14,false);
    int i = 220;
    fd->_write((char *)&i,sizeof(int),true);
    DATA *result = new DATA(true);
    fd->readchk(result, 0, 14);
    string r = (char *)result->data;
    fd->readchk(result, 14, 4);
    int *j = (int *)result->data;
    i = *j;
    cout << r << endl;
    assert(!r.compare("Test passed: "));
    assert(i == 220);
    cout << "Test passed!" << endl;
    delete fd;
    delete result;
}
