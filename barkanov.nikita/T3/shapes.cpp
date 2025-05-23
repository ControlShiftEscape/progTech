#include "shapes.h"

namespace mshapes {
    double getArea(const Polygon& polygon) {
        //шнуровка Гаусса
        const auto& pts = polygon.points;
        if (pts.size() < 3) return 0.0;

        double area = 0.0;
        size_t n = pts.size();

        for (size_t i = 0; i < n; ++i) {
            const Point& p1 = pts[i];
            const Point& p2 = pts[(i + 1) % n];
            area += (p1.x * p2.y) - (p2.x * p1.y);
        }

        return std::abs(area) * 0.5;
    }

    bool isVertexCountEven(const Polygon& polygon) {
        return (polygon.points.size() % 2 == 0);
    }

}
