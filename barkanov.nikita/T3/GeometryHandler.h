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

    enum AREA {
        ODD,
        MEAN,
        ALL
    };

    std::map<std::string, std::function<void(std::istream&, std::ostream&)>> commands_;
    std::vector<mshapes::Polygon> mainVector;

    bool hasPolygons() const {
        return !mainVector.empty();
    }

    double getAreaSum(bool isEven, bool getAll = false) const {
        double area = 0;
        for (auto i : mainVector) {
            if ((isEven == mshapes::isVertexCountEven(i)) || getAll) {
                area += mshapes::getArea(i);
            }
        }
        return area;
    }

    double getMinArea() const {
        double minArea = std::numeric_limits<double>::max();
        for (auto i : mainVector) {
            double localArea = mshapes::getArea(i);
            if (minArea > localArea)
                minArea = localArea;
        }
        return minArea;
    }

    double getMaxArea() const {
        double maxArea = 0;
        for (auto i : mainVector) {
            double localArea = mshapes::getArea(i);
            if (maxArea < localArea)
                maxArea = localArea;
        }
        return maxArea;
    }

    size_t getMinVertexNum() const {
        size_t minVertexNum = std::numeric_limits<size_t>::max();
        for (auto i : mainVector) {
            double localVertexNum = i.points.size();
            if (minVertexNum > localVertexNum)
                minVertexNum = localVertexNum;
        }
        return minVertexNum;
    }

    size_t getMaxVertexNum() const {
        size_t maxVertexNum = 0;
        for (auto i : mainVector) {
            double localVertexNum = i.points.size();
            if (maxVertexNum < localVertexNum)
                maxVertexNum = localVertexNum;
        }

        return maxVertexNum;
    }


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
            iofmtguard guard(out);
            out << std::fixed << std::setprecision(1);
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

                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);
                double area = getAreaSum(false);

                out << area << '\n';
            }
            else if (arg1 == "EVEN") {
                if (!in.eof()) {
                    out << "<UNKNOWN COMMAND>\n";
                    return;
                }
                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);
                double area = getAreaSum(true);

                out << area << '\n';

            }
            else if (arg1 == "MEAN") {
                if (!in.eof()) {
                    out << "<UNKNOWN COMMAND>\n";
                    return;
                }
                if (!hasPolygons()) {
                    out << "ERROR: atleast one polygon required.\n";
                    return;
                }
                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);
                double area = getAreaSum(true, true);

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
                    iofmtguard guard(out);
                    out << std::fixed << std::setprecision(1);
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
                if (!hasPolygons()) {
                    out << "ERROR: atleast one polygon required.\n";
                    return;
                }
                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);

                out << getMaxArea() << '\n';
            }
            else if (arg1 == "VERTEXES") {
                if (!in.eof()) {
                    out << "<UNKNOWN COMMAND>\n";
                    return;
                }
                if (!hasPolygons()) {
                    out << "ERROR: atleast one polygon required.\n";
                    return;
                }

                out << getMaxVertexNum() << '\n';
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
                if (!hasPolygons()) {
                    out << "ERROR: atleast one polygon required.\n";
                    return;
                }
                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);

                out << getMinArea() << '\n';
            }
            else if (arg1 == "VERTEXES") {
                if (!in.eof()) {
                    out << "<UNKNOWN COMMAND>\n";
                    return;
                }
                if (!hasPolygons()) {
                    out << "ERROR: atleast one polygon required.\n";
                    return;
                }

                out << getMinVertexNum() << '\n';
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


    void fillMainVector(const std::string& filename) {

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

    void printMainVector(std::ostream& out) const {
        for (auto i : mainVector) {
            out << i << '\n';
        }
    }

};

#endif
