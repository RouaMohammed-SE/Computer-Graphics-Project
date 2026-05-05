#ifndef CLIPPING_CLIPPER_H
#define CLIPPING_CLIPPER_H

#include "../Shapes/Shape.h"
#include "../Utils/Point.h"

// Provides clipping operations for shapes against supported regions.
class Clipper {
public:
    Clipper();

    void clipAgainstRectangle(Shape* shape, const Point& topLeft, const Point& bottomRight);
    void clipAgainstSquare(Shape* shape, const Point& topLeft, int sideLength);
    void clipAgainstCircle(Shape* shape, const Point& center, int radius);
};

#endif
