#include <iostream>
#include <cassert>
#include <fstream>

#include "fdhandler/fdhandler.hpp"
#include "tests/tests_fdhandler.cpp"

using namespace std;



int main(int argc, char **argv){
    FDHANDLER fd("outfile");
    if(!fd.readchk(1024,5)){
        cout << "Falló como se esperaba.";
    }
    return 0;
}
