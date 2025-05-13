#ifndef GEOMETRYHANDLER_H
#define GEOMETRYHANDLER_H


#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <stdexcept>
#include <iomanip>
#include <limits>

#include "shapes.h"
#include "iofmtguard.h"

class GeometryHandler {
private:

    std::map<std::string, std::function<void(std::istream&, std::ostream&)>> commands_;
    std::vector<mshapes::Polygon> mainVector;

public:
    GeometryHandler() {
        commands_["ECHO"] = [this](std::istream& in = std::cin, std::ostream& out = std::cout) {
            mshapes::Polygon polygon;
            in >> polygon;
            out << polygon << std::endl;

            };
    }

    void execute(const std::string& cmd, std::istream& in, std::ostream& out) {
        if (commands_.find(cmd) != commands_.end()) {
            commands_[cmd](in, out);
        }
        else {
            out << "<UNKNOWN COMMAND>\n";
        }
    }


    void fillMainVector(std::string filename) {

        std::ifstream file(filename);

        if (!file.is_open())
        {
            throw std::invalid_argument("ERROR: Wrong name size!");
        }

        std::vector< mshapes::Polygon > shapes;

        while (!file.eof())
        {
            std::copy(
                std::istream_iterator< mshapes::Polygon >(file),
                std::istream_iterator< mshapes::Polygon >(),
                std::back_inserter(shapes)
            );
            if (file.fail() && !file.eof()) {
                file.clear();
                file.ignore(std::numeric_limits< std::streamsize>::max(), '\n');
            }
        }

        file.close();
        mainVector = std::move(shapes);
    }

    void printMainVector(std::ostream& out) {
        for (auto i : mainVector) {
            out << i << '\n';
        }
    }

};

#endif