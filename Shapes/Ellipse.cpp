#include "Ellipse.h"

Ellipse::Ellipse()
    : Shape(), center(), radiusX(0), radiusY(0), algorithm(EllipseAlgorithmType::Midpoint) {
    // TODO: Add initialization logic if needed.
}

Ellipse::Ellipse(const Point& center, int radiusX, int radiusY, EllipseAlgorithmType algorithm, const Color& color)
    : Shape(color), center(center), radiusX(radiusX), radiusY(radiusY), algorithm(algorithm) {
    // TODO: Add initialization logic if needed.
}

void Ellipse::draw(HDC hdc) {
    // TODO: Call selected ellipse drawing algorithm using the provided WinAPI HDC.
}

// Returns a text representation of the ellipse for saving to a file.
std::string Ellipse::serialize() const {
    return "ELLIPSE "
        + std::to_string(center.x)  + " " + std::to_string(center.y)  + " "
        + std::to_string(radiusX)   + " " + std::to_string(radiusY)   + " "
        + std::to_string(static_cast<int>(algorithm)) + " "
        + std::to_string(color.r)   + " "
        + std::to_string(color.g)   + " "
        + std::to_string(color.b);
}
