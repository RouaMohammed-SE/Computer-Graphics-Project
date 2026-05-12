#ifndef FILLING_FILL_ALGORITHMS_H
#define FILLING_FILL_ALGORITHMS_H

#include "../Utils/Color.h"
#include "../Utils/Point.h"
#include <windows.h>

// Groups area filling algorithm entry points.
class FillAlgorithms {
public:
    FillAlgorithms();

    static void floodFillRecursive(HDC hdc, const Point& start, const Color& fillColor);
    static void floodFillNonRecursive(HDC hdc, const Point& start, const Color& fillColor);
    static void fillCircle(HDC hdc, const Point& center, int radius, const Color& fillColor);
    static void fillCircleWithCircles(
        HDC hdc,
        const Point& center,
        int innerRadius,
        int outerRadius,
        int quarter,
        const Color& startColor,
        const Color& endColor);
    static void fillRectangleWithCurves(HDC hdc, const Point& topLeft, const Point& bottomRight, COLORREF fillColor);
    static void fillSquareWithCurves(HDC hdc, const Point& topLeft, int sideLength, COLORREF fillColor);
};

#endif
