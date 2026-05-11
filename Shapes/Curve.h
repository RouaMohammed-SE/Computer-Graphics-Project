#ifndef SHAPES_CURVE_H
#define SHAPES_CURVE_H

#include "Shape.h"
#include "../Utils/Point.h"
#include <vector>

// Drawable curve shape, intended for Cardinal Spline curves.
class Curve : public Shape {
public:
    Curve();
    Curve(const std::vector<Point>& controlPoints, double tension, const Color& color);

    void draw(HDC hdc) override;

private:
    std::vector<Point> controlPoints;
    double tension;
};

#endif
