#include <iostream>
#include <cassert>
#include <fstream>
#include <string>

#include "fdhandler/fdhandler.hpp"
#include "tests/test_data.cpp"
#include "tests/tests_fdhandler.cpp"

using namespace std;

int main(int argc, char **argv){
    testsuite_fdhandler();
    return 0;
}
