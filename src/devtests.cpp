#include <iostream>
#include <cassert>
#include <fstream>
#include <string>

#include "fdhandler/fdhandler.hpp"
#include "tests/test_data.cpp"
#include "tests/tests_fdhandler.cpp"

using namespace std;

int main(int argc, char **argv){
    test_fdhandler_append("testfile");
    test_fdhandler_append2("testfile");
    test_fdhandler_write("testfile");
    test_fdhandler_write2("testfile");
    cout << "All tests passed." << endl;
}
