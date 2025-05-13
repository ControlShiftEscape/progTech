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
        commands_["GET_AREA"] = [this](std::istream& in, std::ostream& out) {
            mshapes::Polygon polygon;
            in >> polygon;
            if (!in.eof()) {
                out << "<UNKNOWN COMMAND>\n";
                return;
            }
            out << mshapes::getArea(polygon) << std::endl;
            };
        commands_["AREA"] = [this](std::istream& in, std::ostream& out) {
            mshapes::Polygon polygon;
            std::string arg1;
            in >> arg1;
            if (arg1 == "ODD") {
                if (!in.eof()) {
                    out << "<UNKNOWN COMMAND>\n";
                    return;
                }

                double area = 0;
                out << std::fixed << std::setprecision(1);
                for (auto i : mainVector) {
                    if (!mshapes::isVertexCountEven(i)) {
                        out << mshapes::getArea(i) << '\n';
                        area += mshapes::getArea(i);
                    }
                }

                out << area << '\n';
            }
            else if (arg1 == "EVEN") {
                if (!in.eof()) {
                    out << "<UNKNOWN COMMAND>\n";
                    return;
                }
                double area = 0;
                out << std::fixed << std::setprecision(1);
                for (auto i : mainVector) {
                    if (mshapes::isVertexCountEven(i)) {
                        out << mshapes::getArea(i) << '\n';
                        area += mshapes::getArea(i);
                    }
                }

                out << area << '\n';

            }
            else if (arg1 == "MEAN") {
                if (!in.eof()) {
                    out << "<UNKNOWN COMMAND>\n";
                    return;
                }
                if (mainVector.size() < 1) {
                    out << "ERROR: atleast one polygon required.\n";
                    return;
                }
                double area = 0;
                out << std::fixed << std::setprecision(1);
                for (auto i : mainVector) {
                    area += mshapes::getArea(i);
                }

                out << area / mainVector.size() << '\n';
            }
            else {
                double num = 0;
                try {
                    num = std::stoi(arg1);
                }
                catch (...) {
                    out << "<UNKNOWN COMMAND>\n";
                }
                if (in.eof()) {
                    double area = 0;
                    for (auto i : mainVector) {
                        if (i.points.size() == num)
                            area += mshapes::getArea(i);
                    }
                    out << area << '\n';
                }
                else
                    out << "<UNKNOWN COMMAND>\n";
                return;
            }

            };
        commands_["MAX"] = [this](std::istream& in, std::ostream& out) {
            std::string arg1;
            in >> arg1;
            if (arg1 == "AREA") {
                if (!in.eof()) {
                    out << "<UNKNOWN COMMAND>\n";
                    return;
                }
                if (mainVector.size() < 1) {
                    out << "ERROR: atleast one polygon required.\n";
                    return;
                }
                double maxArea = 0;
                for (auto i : mainVector) {
                    double localArea = mshapes::getArea(i);
                    if (maxArea < localArea)
                        maxArea = localArea;
                }

                out << maxArea << '\n';
            }
            else if (arg1 == "VERTEXES") {
                if (!in.eof()) {
                    out << "<UNKNOWN COMMAND>\n";
                    return;
                }
                if (mainVector.size() < 1) {
                    out << "ERROR: atleast one polygon required.\n";
                    return;
                }
                size_t maxVertexNum = 0;
                for (auto i : mainVector) {
                    double localVertexNum = i.points.size();
                    if (maxVertexNum < localVertexNum)
                        maxVertexNum = localVertexNum;
                }

                out << maxVertexNum << '\n';
            }
            };
        commands_["MIN"] = [this](std::istream& in, std::ostream& out) {
            std::string arg1;
            in >> arg1;
            if (arg1 == "AREA") {
                if (!in.eof()) {
                    out << "<UNKNOWN COMMAND>\n";
                    return;
                }
                if (mainVector.size() < 1) {
                    out << "ERROR: atleast one polygon required.\n";
                    return;
                }
                double minArea = std::numeric_limits<double>::max();;
                for (auto i : mainVector) {
                    double localArea = mshapes::getArea(i);
                    if (minArea > localArea)
                        minArea = localArea;
                }

                out << minArea << '\n';
            }
            else if (arg1 == "VERTEXES") {
                if (!in.eof()) {
                    out << "<UNKNOWN COMMAND>\n";
                    return;
                }
                if (mainVector.size() < 1) {
                    out << "ERROR: atleast one polygon required.\n";
                    return;
                }
                size_t minVertexNum = std::numeric_limits<size_t>::max();;
                for (auto i : mainVector) {
                    double localVertexNum = i.points.size();
                    if (minVertexNum > localVertexNum)
                        minVertexNum = localVertexNum;
                }

                out << minVertexNum << '\n';
            }
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
