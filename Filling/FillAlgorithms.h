#ifndef FILLING_FILL_ALGORITHMS_H
#define FILLING_FILL_ALGORITHMS_H

#include "../Utils/Color.h"
#include "../Utils/Point.h"

// Groups area filling algorithm entry points.
class FillAlgorithms {
public:
    FillAlgorithms();

    static void floodFillRecursive(const Point& start, const Color& fillColor);
    static void floodFillNonRecursive(const Point& start, const Color& fillColor);
    static void fillCircle(const Point& center, int radius, const Color& fillColor);
    static void fillRectangleWithCurves(const Point& topLeft, const Point& bottomRight, const Color& fillColor);
    static void fillSquareWithCurves(const Point& topLeft, int sideLength, const Color& fillColor);
};

#endif
