
#ifndef SHAPES_H
#define SHAPES_H

#include <vector>
#include "delimiters.h"
#include "iofmtguard.h"

const size_t MIN_VERTEX_AMOUNT = 3;

namespace mshapes {

    struct Point {
        int x, y;
    };

    inline std::istream& operator>> (std::istream& in, Point& point) {
        std::istream::sentry sentry(in);

        if (!sentry)
            return in;

        Point input;
        in >> DelimiterIO{ '(' } >> input.x >> DelimiterIO{ ';' } >> input.y >> DelimiterIO{ ')' };

        if (!in.fail())
            point = input;
        return in;

    }

    inline std::ostream& operator<<(std::ostream& out, const mshapes::Point& point) {
        out << '(' << point.x << ';' << point.y << ')';
        return out;
    }

    struct Polygon {
        std::vector<Point> points;
    };

    double getArea(const Polygon& polygon);

    bool isVertexCountEven(const Polygon& polygon);

    inline std::istream& operator>> (std::istream& in, Polygon& polygon) {
        std::istream::sentry sentry(in);

        if (!sentry)
            return in;

        Polygon polygonInput;
        size_t vertexNumber = 0;

        in >> vertexNumber;
        if (!in || vertexNumber < MIN_VERTEX_AMOUNT) {
            in.setstate(std::ios::failbit);
            return in;
        }

        Point pointInput;

        for (size_t i = 0; i < vertexNumber; ++i) {
            in >> pointInput;
            if (!in)
                return in;
            polygonInput.points.push_back(pointInput);
        }

        polygon = polygonInput;
        return in;
    }

    inline std::ostream& operator<<(std::ostream& out, const Polygon& polygon) {
        out << polygon.points.size();
        for (auto i : polygon.points) {
            out << ' ' << i;
        }
        return out;
    }

}

#endif
