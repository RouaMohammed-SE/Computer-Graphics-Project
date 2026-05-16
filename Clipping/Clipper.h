#pragma once
#ifndef CLIPPING_CLIPPER_H
#define CLIPPING_CLIPPER_H

#include "../Utils/Point.h"
#include "../Algorithms/LineAlgorithms.h"
#include <vector>

using namespace std;

// Provides clipping operations for shapes against supported regions.
class Clipper {
public:
    Clipper();

    void rectanglePointClipping(HDC hdc, Point &topleft , Point &bottomright , Point& point , COLORREF c);
    void rectangleLineClipping(HDC hdc, Point &topleft , Point &bottomright , Point& start , Point& end , COLORREF c);
    void rectanglePolygonClipping(HDC hdc , Point& topleft , Point& bottomright , vector<Point>& P);
    void squarePointClipping(HDC hdc, Point &topleft , int sideLength, Point& point , COLORREF c);
    void squareLineClipping(HDC hdc, Point &topleft , int sideLength , Point& start , Point& end , COLORREF c);
    void circlePointClipping(HDC hdc, Point center, int radius, Point point, COLORREF c);
    void circleLineClipping(HDC hdc, Point center, int radius, Point point1, Point point2, COLORREF c);
};

#endif
