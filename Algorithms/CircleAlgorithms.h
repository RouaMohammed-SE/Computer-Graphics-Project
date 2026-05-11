#ifndef ALGORITHMS_CIRCLE_ALGORITHMS_H
#define ALGORITHMS_CIRCLE_ALGORITHMS_H

#include "../Utils/Point.h"
#include <windows.h>

// Groups circle drawing algorithm entry points.
class CircleAlgorithms {
private:
	void draw8Points(HDC hdc, Point& center, Point& p, COLORREF color);
public:
    CircleAlgorithms();

    static void drawDirect(HDC hdc, const Point& center, int radius, COLORREF color);
    static void drawPolar(HDC hdc, const Point& center, int radius, COLORREF color);
    static void drawIterativePolar(HDC hdc, const Point& center, int radius, COLORREF color);
    static void drawMidpoint(HDC hdc, const Point& center, int radius, COLORREF color);
    static void drawModifiedMidpoint(HDC hdc, const Point& center, int radius, COLORREF color);
};

#endif
