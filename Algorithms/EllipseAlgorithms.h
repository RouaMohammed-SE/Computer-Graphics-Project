#ifndef ALGORITHMS_ELLIPSE_ALGORITHMS_H
#define ALGORITHMS_ELLIPSE_ALGORITHMS_H

#include "../Utils/Point.h"
#include <windows.h>

// Groups ellipse drawing algorithm entry points.
class EllipseAlgorithms {
public:
    EllipseAlgorithms();

    static void drawDirect(HDC hdc, const Point& center, int radiusX, int radiusY, COLORREF color);
    static void drawPolar(HDC hdc, const Point& center, int radiusX, int radiusY, COLORREF color);
    static void drawMidpoint(HDC hdc, const Point& center, int radiusX, int radiusY, COLORREF color);
};

#endif
