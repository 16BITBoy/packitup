#include <iostream>
#include <cassert>
#include <fstream>

#include "fdhandler/fdhandler.hpp"

using namespace std;

void test_data(){
    // Silly tests for a silly class :D
    // Empty object should be not null but empty (Silly test yeah xD)
    DATA *data = new DATA(); 
    assert(data != NULL);
    assert(data->data == NULL);
    assert(data->size == 0);
    delete data;
    
    // Object pointing to int value.
    int i = 453;
    data = new DATA(&i, sizeof i);
    assert(data != NULL);
    assert(data->data == &i);
    assert(data->size == sizeof i);
    delete data;
    
    // Object pointing to dynamically allocated int
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
