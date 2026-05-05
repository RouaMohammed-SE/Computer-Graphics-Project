#ifndef SHAPES_SHAPE_H
#define SHAPES_SHAPE_H

#include "../Utils/Color.h"

// Abstract base class for every drawable shape in the application.
class Shape {
public:
    Shape();
    explicit Shape(const Color& color);
    virtual ~Shape();

    virtual void draw() = 0;
    Color getColor() const;
    void setColor(const Color& color);

protected:
    Color color;
};

#endif
