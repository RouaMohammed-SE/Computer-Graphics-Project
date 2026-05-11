#ifndef ALGORITHMS_CURVE_ALGORITHMS_H
#define ALGORITHMS_CURVE_ALGORITHMS_H

#include "../Utils/Point.h"
#include <windows.h>
#include <vector>

// Groups curve drawing algorithm entry points.
class CurveAlgorithms {
public:
    CurveAlgorithms();

    static void drawCardinalSpline(HDC hdc, const std::vector<Point>& points, double tension, COLORREF color);
};

#endif
