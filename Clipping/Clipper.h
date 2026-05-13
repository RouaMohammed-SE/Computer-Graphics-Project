#pragma once
#ifndef CLIPPING_CLIPPER_H
#define CLIPPING_CLIPPER_H

#include "../Shapes/Shape.h"
#include "../Utils/Point.h"

// Provides clipping operations for shapes against supported regions.
class Clipper {
public:
    Clipper();

    void quadPointClipping(HDC hdc, Point &topleft , Point &bottomright , Point& point , COLORREF c);
    void quadLineClipping(HDC hdc, Point &topleft , Point &bottomright , Point& start , Point& end , COLORREF c);
    void clipAgainstRectangle(Shape* shape, const Point& topLeft, const Point& bottomRight);
    void clipAgainstSquare(Shape* shape, const Point& topLeft, int sideLength);
    void clipAgainstCircle(Shape* shape, const Point& center, int radius);
    void circlePointClipping(HDC hdc, Point &center, int radius, Point &point, COLORREF c);
    void circleLineClipping(HDC hdc, Point &center, int radius, Point &point1, Point &point2, COLORREF c);
};

#endif
