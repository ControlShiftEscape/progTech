#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream> //string stream

#include "shapes.h"
#include "CommandHandler.h"

/*
    variant 6
    ECHO MAXSEQ
*/

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "ERROR: Exactly one argument must be passed.";
        return 1;
    }
    std::string filename;
    filename = argv[1];

    std::string line;
    GeometryHandler cmdHandler;

    cmdHandler.fillMainVector(filename);

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        cmdHandler.execute(cmd, iss, std::cout);
    }

    return 0;
}
