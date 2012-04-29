#include <cassert>
#include <iostream>

#include "../fdhandler/fdhandler.hpp"

using namespace std;

void test_data(){
    // Silly tests for a silly class :D
    // Empty object should be not null but empty (Silly test yeah xD)
    DATA *data = new DATA(false);
    assert(data != NULL);
    assert(data->data == NULL);
    assert(data->size == 0);
    delete data;
    
    // Object pointing to int value.
    int i = 453;
    data = new DATA(&i, sizeof i, false);
    assert(data != NULL);
    assert(data->data == &i);
    assert(data->size == sizeof i);
    delete data;
    
    // Object pointing to dynamically allocated int
    int *a = new int[3];
    data = new DATA(a, sizeof(int)*3, true);
    assert(data != NULL);
    assert(data->data == a);
    assert(data->size == sizeof(int)*3);
    delete data;
    cout << "Tests passed for DATA class." << endl;
}
