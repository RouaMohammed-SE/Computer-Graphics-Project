#ifndef ALGORITHMS_ELLIPSE_ALGORITHMS_H
#define ALGORITHMS_ELLIPSE_ALGORITHMS_H

#include "../Utils/Point.h"

// Groups ellipse drawing algorithm entry points.
class EllipseAlgorithms {
public:
    EllipseAlgorithms();

    static void drawDirect(const Point& center, int radiusX, int radiusY);
    static void drawPolar(const Point& center, int radiusX, int radiusY);
    static void drawMidpoint(const Point& center, int radiusX, int radiusY);
};

#endif
