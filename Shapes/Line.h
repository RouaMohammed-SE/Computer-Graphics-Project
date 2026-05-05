#ifndef SHAPES_LINE_H
#define SHAPES_LINE_H

#include "Shape.h"
#include "../Core/Enums.h"
#include "../Utils/Point.h"

// Drawable line shape configured with a selected line algorithm.
class Line : public Shape {
public:
    Line();
    Line(const Point& start, const Point& end, LineAlgorithmType algorithm, const Color& color);

    void draw() override;

private:
    Point start;
    Point end;
    LineAlgorithmType algorithm;
};

#endif
