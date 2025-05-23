#ifndef GEOMETRYHANDLER_H
#define GEOMETRYHANDLER_H

//command aliases
#define UNKNOWN_COMMAND "INVALID COMMAND"
#define E_INCORRECT_INPUT "INVALID COMMAND"
//#define E_INCORRECT_INPUT "ERROR: INCORRECT INPUT"

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
#include <numeric>
#include <functional>

#include <algorithm>

#include "shapes.h"
#include "iofmtguard.h"

class GeometryHandler {
private:

    std::map<std::string, std::function<void(std::istream&, std::ostream&)>> commands_;
    std::vector<mshapes::Polygon> mainVector;

    bool hasPolygons() const {
        return !mainVector.empty();
    }

    double getAreaSum(bool isEven, bool getAll = false) const {
        return std::accumulate(mainVector.begin(), mainVector.end(), 0.0,
            [isEven, getAll](double sum, const auto& polygon) {
                if ((isEven == mshapes::isVertexCountEven(polygon)) || getAll) {
                    return sum + mshapes::getArea(polygon);
                }
                return sum;
            }

        );
    }

    size_t countPolygons(bool isEven) const {
        return std::count_if(mainVector.begin(), mainVector.end(),
            [isEven](const auto& i) {
                return isEven == mshapes::isVertexCountEven(i);
            }
        );
    }

    size_t countPolygons(size_t amount) const {
        return std::count_if(
            mainVector.begin(), mainVector.end(),
            std::bind(
                hasGivenSize,
                std::placeholders::_1, amount
            )
        );
    }

    static bool hasGivenSize(const mshapes::Polygon& polygon, size_t vertexCount) {
        return polygon.points.size() == vertexCount;
    }

    double getMinArea() const {
        auto it = std::min_element(mainVector.begin(), mainVector.end(),
            [](const auto& i1, const auto& i2) {
                return mshapes::getArea(i1) < mshapes::getArea(i2);
            }
        );
        if (it != mainVector.end()) {
            return mshapes::getArea(*it);
        }
        return 0;
    }

    double getMaxArea() const {
        auto it = std::max_element(mainVector.begin(), mainVector.end(),
            [](const auto& i1, const auto& i2) {
                return mshapes::getArea(i1) < mshapes::getArea(i2);
            }
        );
        if (it != mainVector.end()) {
            return mshapes::getArea(*it);
        }
        return 0;
    }

    double getAreaWithVertexNum(size_t num) const {
        return std::accumulate(mainVector.begin(), mainVector.end(), 0.0,
            [num](double area, const auto& polygon) {
                area += (polygon.points.size() == num) ? mshapes::getArea(polygon) : 0;
                return area;
            });
    }

    size_t getMinVertexNum() const {
        auto it = std::min_element(mainVector.begin(), mainVector.end(),
            [](const auto& i1, const auto& i2) {
                return i1.points.size() < i2.points.size();
            }
        );
        if (it != mainVector.end()) {
            return (*it).points.size();
        }
        return 0;
    }

    size_t getMaxVertexNum() const {
        auto it = std::max_element(mainVector.begin(), mainVector.end(),
            [](const auto& i1, const auto& i2) {
                return i1.points.size() < i2.points.size();
            }
        );
        if (it != mainVector.end()) {
            return (*it).points.size();
        }
        return 0;
    }

    void registerCommands() {
        commands_["MANUAL"] = [this](std::istream& in, std::ostream& out) {
            std::vector<std::string> vec;

            if (in.peek() != EOF) {
                out << "<" << UNKNOWN_COMMAND << ">\n";
                return;
            }
            std::transform(commands_.begin(), commands_.end(), std::back_inserter(vec),
                [](const auto& pair) {
                    return pair.first;
                }
            );
            out << "---SUPPORTED COMMANDS---\n";
            std::copy(vec.begin(), vec.end(), std::ostream_iterator<std::string>(out, "\n"));
            out << "------------------------\n";
            };
        commands_["GET_AREA"] = [this](std::istream& in, std::ostream& out) {
            mshapes::Polygon polygon;
            in >> polygon;
            if (!in) {
                out << "<" << E_INCORRECT_INPUT << ">\n";
                return;
            }
            if (in.peek() != EOF) {
                out << "<" << UNKNOWN_COMMAND << ">\n";
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
                if (in.peek() != EOF) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }

                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);
                double area = getAreaSum(false);

                out << area << '\n';
            }
            else if (arg1 == "EVEN") {
                if (in.peek() != EOF) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);
                double area = getAreaSum(true);

                out << area << '\n';

            }
            else if (arg1 == "MEAN") {
                if (in.peek() != EOF) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                if (!hasPolygons()) {
                    out << '<' << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);
                double area = getAreaSum(true, true);

                out << area / mainVector.size() << '\n';
            }
            else {
                size_t num = 0;
                try {
                    num = std::stoul(arg1);
                }
                catch (...) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                if (num < 3 || in.peek() != EOF) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }

                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);

                out << getAreaWithVertexNum(num) << '\n';

                return;
            }

            };
        commands_["MAX"] = [this](std::istream& in, std::ostream& out) {
            std::string arg1;
            in >> arg1;
            if (arg1 == "AREA") {
                if (in.peek() != EOF) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                if (!hasPolygons()) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);

                out << getMaxArea() << '\n';
            }
            else if (arg1 == "VERTEXES") {
                if (in.peek() != EOF) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                if (!hasPolygons()) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }

                out << getMaxVertexNum() << '\n';
            }
            };
        commands_["MIN"] = [this](std::istream& in, std::ostream& out) {
            std::string arg1;
            in >> arg1;
            if (arg1 == "AREA") {
                if (in.peek() != EOF) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                if (!hasPolygons()) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);

                out << getMinArea() << '\n';
            }
            else if (arg1 == "VERTEXES") {
                if (in.peek() != EOF) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                if (!hasPolygons()) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }

                out << getMinVertexNum() << '\n';
            }
            };
        commands_["COUNT"] = [this](std::istream& in, std::ostream& out) {
            mshapes::Polygon polygon;
            std::string arg1;
            in >> arg1;
            if (arg1 == "ODD") {
                if (in.peek() != EOF) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }

                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);

                out << countPolygons(false) << '\n';
            }
            else if (arg1 == "EVEN") {
                if (in.peek() != EOF) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                iofmtguard guard(out);
                out << std::fixed << std::setprecision(1);

                out << countPolygons(true) << '\n';

            }
            else {
                size_t num = 0;
                try {
                    num = std::stoul(arg1);
                }
                catch (...) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }
                if (num < 3 || in.peek() != EOF) {
                    out << "<" << UNKNOWN_COMMAND << ">\n";
                    return;
                }

                out << countPolygons(num) << '\n';

                return;
            }

            };
        commands_["MAXSEQ"] = [this](std::istream& in, std::ostream& out) {
            mshapes::Polygon polygon;
            in >> polygon;
            if ((!in) || (in.peek() != EOF)) {
                out << "<" << E_INCORRECT_INPUT << ">\n";
                return;
            }
            size_t maxCount = 0;
            std::accumulate(mainVector.begin(), mainVector.end(), 0,
                [&maxCount, polygon](size_t count, const auto& i) {
                    if (i == polygon) {
                        return static_cast<size_t>(count + 1);
                    }
                    else {
                        maxCount = (maxCount < count) ? count : maxCount;
                        return static_cast<size_t>(0);
                    }
                }
            );

            out << maxCount << '\n';;
            };
        commands_["ECHO"] = [this](std::istream& in, std::ostream& out) {
            mshapes::Polygon polygon;
            in >> polygon;
            if ((!in) || (in.peek() != EOF)) {
                out << "<" << E_INCORRECT_INPUT << '>' << '\n';
                return;
            }

            std::vector<mshapes::Polygon> newMainVector;
            newMainVector.reserve(mainVector.size());
            try {
                std::accumulate(mainVector.begin(), mainVector.end(), std::ref(newMainVector),
                    [&polygon](std::vector<mshapes::Polygon>& acc, const auto& curPolygon) -> std::vector<mshapes::Polygon>&{
                        acc.push_back(curPolygon);
                        if (curPolygon == polygon)
                            acc.push_back(curPolygon);
                        return acc;
                    }
                );
            }
            catch (const std::exception& e) {
                out << "<ERROR: " << e.what() << '>' << '\n';
                return;
            }
            size_t duplicationCount = newMainVector.size() - mainVector.size();
            mainVector = std::move(newMainVector);
            out << duplicationCount << '\n';
            };
    }



public:

    GeometryHandler() {
        registerCommands();
    }

    void processCommandLine(std::istream& in) {
        std::string cmd;
        in >> cmd;
        execute(cmd, in, std::cout);
    }

    void execute(const std::string& cmd, std::istream& in, std::ostream& out) {
        if (commands_.find(cmd) != commands_.end()) {
            commands_[cmd](in, out);
        }
        else {
            out << "<" << UNKNOWN_COMMAND << ">\n";
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
        std::copy(mainVector.begin(), mainVector.end(),
            std::ostream_iterator<mshapes::Polygon>(out, "\n"));
    }

};

#endif
