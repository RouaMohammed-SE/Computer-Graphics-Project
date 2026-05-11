#include "Shape.h"

Shape::Shape() : color() {
}

Shape::Shape(const Color& color) : color(color) {
}

Shape::~Shape() {
}

Color Shape::getColor() const {
    return color;
}

void Shape::setColor(const Color& color) {
    this->color = color;
}
