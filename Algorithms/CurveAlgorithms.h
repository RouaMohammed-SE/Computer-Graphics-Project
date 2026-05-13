#pragma once
#ifndef ALGORITHMS_CURVE_ALGORITHMS_H
#define ALGORITHMS_CURVE_ALGORITHMS_H

#include "../Utils/Point.h"
#include <windows.h>
#include <vector>

// Groups curve drawing algorithm entry points.
class CurveAlgorithms {
public:
    CurveAlgorithms();

    static void bezierCurve(HDC hdc, Point& p1, Point& p2, Point& p3, Point& p4, COLORREF color);
    static void hermiteCurve(HDC hdc, const Point& p1, const Point& s1, const Point& p2, const Point& s2, COLORREF color);
    static void drawCardinalSpline(HDC hdc, const std::vector<Point>& points, double tension, COLORREF color);
};

#endif
