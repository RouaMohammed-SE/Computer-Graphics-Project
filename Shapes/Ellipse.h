#ifndef SHAPES_ELLIPSE_H
#define SHAPES_ELLIPSE_H

#include "Shape.h"
#include "../Core/Enums.h"
#include "../Utils/Point.h"

// Drawable ellipse shape configured with a selected ellipse algorithm.
class Ellipse : public Shape {
public:
    Ellipse();
    Ellipse(const Point& center, int radiusX, int radiusY, EllipseAlgorithmType algorithm, const Color& color);

    void draw(HDC hdc) override;

private:
    Point center;
    int radiusX;
    int radiusY;
    EllipseAlgorithmType algorithm;
};

#endif
