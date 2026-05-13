#include "Ellipse.h"
#include "../Algorithms/EllipseAlgorithms.h"

namespace {
COLORREF toColorRef(const Color& color) {
    return RGB(color.r, color.g, color.b);
}
}

MyEllipse::MyEllipse()
    : Shape(), center(), radiusX(0), radiusY(0), algorithm(EllipseAlgorithmType::Midpoint) {
    // TODO: Add initialization logic if needed.
}

MyEllipse::MyEllipse(const Point& center, int radiusX, int radiusY, EllipseAlgorithmType algorithm, const Color& color)
    : Shape(color), center(center), radiusX(radiusX), radiusY(radiusY), algorithm(algorithm) {
    // TODO: Add initialization logic if needed.
}

void MyEllipse::draw(HDC hdc) {
    switch (algorithm) {
    case EllipseAlgorithmType::Direct:
        EllipseAlgorithms::drawDirect(hdc, center, radiusX, radiusY, toColorRef(color));
        break;
    case EllipseAlgorithmType::Polar:
        EllipseAlgorithms::drawPolar(hdc, center, radiusX, radiusY, toColorRef(color));
        break;
    case EllipseAlgorithmType::IterativePolar:
        EllipseAlgorithms::drawIterativePolar(hdc, center, radiusX, radiusY, toColorRef(color));
        break;
    case EllipseAlgorithmType::Midpoint:
        EllipseAlgorithms::drawMidpoint(hdc, center, radiusX, radiusY, toColorRef(color));
        break;
    }
}

// Returns a text representation of the ellipse for saving to a file.
std::string MyEllipse::serialize() const {
    return "ELLIPSE "
        + std::to_string(center.x)  + " " + std::to_string(center.y)  + " "
        + std::to_string(radiusX)   + " " + std::to_string(radiusY)   + " "
        + std::to_string(static_cast<int>(algorithm)) + " "
        + std::to_string(color.r)   + " "
        + std::to_string(color.g)   + " "
        + std::to_string(color.b);
}
