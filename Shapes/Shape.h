#pragma once
#ifndef SHAPES_SHAPE_H
#define SHAPES_SHAPE_H

#include "../Utils/Color.h"
#include <windows.h>
#include <string>

// Abstract base class for every drawable shape in the application.
class Shape {
public:
    Shape();
    explicit Shape(const Color& color);
    virtual ~Shape();

    virtual void draw(HDC hdc) = 0;

    // Returns a text representation of the shape for saving to a file.
    virtual std::string serialize() const = 0;
    Color getColor() const;
    void setColor(const Color& color);

protected:
    Color color;
};

#endif
