#ifndef SHAPES_CIRCLE_H
#define SHAPES_CIRCLE_H

#include "Shape.h"
#include "../Core/Enums.h"
#include "../Utils/Point.h"
#include <string>

// Drawable circle shape configured with a selected circle algorithm.
class Circle : public Shape {
public:
    Circle();
    Circle(const Point& center, int radius, CircleAlgorithmType algorithm, const Color& color);

    void draw(HDC hdc) override;

    // Returns a text representation of the circle for saving to a file.
    std::string serialize() const override;

private:
    Point center;
    int radius;
    CircleAlgorithmType algorithm;
};

#endif
