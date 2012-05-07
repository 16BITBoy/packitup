#include <iostream>
#include <cassert>
#include <fstream>

#include "../fdhandler/fdhandler.hpp"

using namespace std;

/* Manual test for readall function */
void test_fdhandler_readall(string file){ //Needs an actual file to test
    FDHANDLER fd(file);
    DATA *filedata = new DATA();
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
    DATA *filedata = new DATA();
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
    cout << "Testing FDHANDLER::write(DATA *)" << endl;
    cout << "---------------------------------" << endl;
    FDHANDLER fd(outfile);
    DATA *outbuffer = new DATA(new char[1024], 1024);
    unsigned long i;
    char *c;
    c = (char*)outbuffer->data;
    for(i = 0; i < 1024; i++){
        *(c + i) = 7;
    }
    fd.write(outbuffer);
    DATA *inbuffer = new DATA();
    char *c2;
    fd.readall(inbuffer);
    c2 = (char *)inbuffer->data;
    for(i = 0; i < 1024; i++){
        assert(*(c + i) == *(c2 + i));
    }
    assert(!fd.error());
    cout << "Test passed!" << endl << endl;
    delete outbuffer;
    delete inbuffer;
}

void test_fdhandler_write2(string outfile){//output filename
    cout << "Testing FDHANDLER::write(char *, unsigned long)" << endl;
    cout << "---------------------------------" << endl;
    FDHANDLER fd(outfile);
    unsigned long i;
    char *c = new char[1024];
    for(i = 0; i < 1024; i++){
        *(c + i) = 7;
    }
    fd.write(c,1024);
    DATA *inbuffer = new DATA();
    char *c2;
    fd.readall(inbuffer);
    c2 = (char *)inbuffer->data;
    for(i = 0; i < 1024; i++){
        assert(*(c + i) == *(c2 + i));
    }
    assert(!fd.error());
    cout << "Test passed!" << endl << endl;
    delete c;
    delete inbuffer;
}

void test_fdhandler_append(string targetfile){
    cout << "Testing FDHANDLER::append(DATA *)" << endl;
    cout << "---------------------------------" << endl;
    FDHANDLER *fd = new FDHANDLER(targetfile);
    string *s = new string();
    *s = "Test passed: ";
    fd->write(new DATA((void *)s->c_str(),13));
    string *s2 = new string();
    *s2 = "OK";
    fd->append(new DATA((void *)s2->c_str(),3));
    DATA *result = new DATA();
    fd->readall(result);
    string r = (char *)result->data ;
    cout << r << endl;
    assert(!r.compare("Test passed: OK"));
    cout << "Test passed!" << endl << endl;
    delete fd;
    delete result;
    delete s;
    delete s2;
}

void test_fdhandler_append2(string targetfile){
    cout << "Testing FDHANDLER::append(char *, unsigned long)" << endl;
    cout << "---------------------------------" << endl;
    FDHANDLER *fd = new FDHANDLER(targetfile);
    string s = "Test passed: ";
    fd->write((char *)s.c_str(),14);
    int i = 220;
    fd->append((char *)&i,sizeof(int));
    DATA *result = new DATA();
    fd->readchk(result, 0, 14);
    string r = (char *)result->data;
    fd->readchk(result, 14, 4);
    int *j = (int *)result->data;
    i = *j;
    assert(!r.compare("Test passed: "));
    assert(i == 220);
    cout << "Test passed!" << endl << endl;
    delete fd;
    delete result;
}
