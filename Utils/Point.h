#pragma once
#ifndef UTILS_POINT_H
#define UTILS_POINT_H

// Represents a 2D point in screen coordinates.
class Point {
public:
    int x;
    int y;

    Point();
    Point(int x, int y);
};

#endif
