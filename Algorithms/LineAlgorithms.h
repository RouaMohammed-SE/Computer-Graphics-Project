#pragma once
#ifndef ALGORITHMS_LINE_ALGORITHMS_H
#define ALGORITHMS_LINE_ALGORITHMS_H

#include "../Utils/Point.h"
#include <windows.h>

// Groups line drawing algorithm entry points.
class LineAlgorithms {
public:
    LineAlgorithms();

    static void drawDDA(HDC hdc, const Point& start, const Point& end, COLORREF color);
    static void drawMidpoint(HDC hdc, const Point& start, const Point& end, COLORREF color);
    static void drawParametric(HDC hdc, const Point& start, const Point& end, COLORREF color);
};

#endif
