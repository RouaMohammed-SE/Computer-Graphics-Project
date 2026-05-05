#include "Ellipse.h"

Ellipse::Ellipse()
    : Shape(), center(), radiusX(0), radiusY(0), algorithm(EllipseAlgorithmType::Midpoint) {
    // TODO: Add initialization logic if needed.
}

Ellipse::Ellipse(const Point& center, int radiusX, int radiusY, EllipseAlgorithmType algorithm, const Color& color)
    : Shape(color), center(center), radiusX(radiusX), radiusY(radiusY), algorithm(algorithm) {
    // TODO: Add initialization logic if needed.
}

void Ellipse::draw() {
    // TODO: Call selected ellipse drawing algorithm.
}
