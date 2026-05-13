#pragma once
#ifndef FILLING_FILL_ALGORITHMS_H
#define FILLING_FILL_ALGORITHMS_H

#include "../Utils/Color.h"
#include "../Utils/Point.h"
#include <windows.h>
#include <vector>
using namespace std;

// Groups area filling algorithm entry points.
class FillAlgorithms {
public:
    FillAlgorithms();

    static void floodFillRecursive(HDC hdc, const Point& start, const Color& fillColor , const Color& borderColor);
    static void floodFillNonRecursive(HDC hdc, const Point& start, const Color& fillColor, const Color& borderColor);
    static void fillCircle(HDC hdc, const Point& center, int radius, const Color& fillColor);
    static void fillCircleWithLines(HDC hdc, const Point& center, int radius, int quarter, const Color& fillColor);
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
    static void convexFill(HDC hdc,vector<Point> p,int n, COLORREF color);
    static void nonConvexFill(HDC hdc, vector<Point> p, int n, COLORREF c);
};

#endif
