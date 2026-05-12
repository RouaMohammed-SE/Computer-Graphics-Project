#ifndef SHAPES_ELLIPSE_H
#define SHAPES_ELLIPSE_H

#include "Shape.h"
#include "../Core/Enums.h"
#include "../Utils/Point.h"
#include <string>

// Drawable ellipse shape configured with a selected ellipse algorithm.
class MyEllipse : public Shape {
public:
    MyEllipse();
    MyEllipse(const Point& center, int radiusX, int radiusY, EllipseAlgorithmType algorithm, const Color& color);

    void draw(HDC hdc) override;

    // Returns a text representation of the ellipse for saving to a file.
    std::string serialize() const override;

private:
    Point center;
    int radiusX;
    int radiusY;
    EllipseAlgorithmType algorithm;
};

#endif
