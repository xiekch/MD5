#include "md5.h"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "\nparamter error\n");
        exit(1);
    }
    const char *inputFilename = argv[1];
    const char *outputFilename = argv[2];
    MD5 m;
    m.md5(inputFilename, outputFilename);

    return 0;
}
