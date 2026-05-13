#include "Line.h"
#include "../Algorithms/LineAlgorithms.h"
#include <string>

namespace {
COLORREF toColorRef(const Color& color) {
    return RGB(color.r, color.g, color.b);
}
}

Line::Line() : Shape(), start(), end(), algorithm(LineAlgorithmType::DDA) {
    // TODO: Add initialization logic if needed.
}

Line::Line(const Point& start, const Point& end, LineAlgorithmType algorithm, const Color& color)
    : Shape(color), start(start), end(end), algorithm(algorithm) {
    // TODO: Add initialization logic if needed.
}

void Line::draw(HDC hdc) {
    switch (algorithm) {
    case LineAlgorithmType::DDA:
        LineAlgorithms::drawDDA(hdc, start, end, toColorRef(color));
        break;
    case LineAlgorithmType::Midpoint:
        LineAlgorithms::drawMidpoint(hdc, start, end, toColorRef(color));
        break;
    case LineAlgorithmType::Parametric:
        LineAlgorithms::drawParametric(hdc, start, end, toColorRef(color));
        break;
    }
}

// Returns a text representation of the line for saving to a file.
std::string Line::serialize() const {
    // Implement serialization logic for the line.
    return "LINE "
        + std::to_string(start.x)  + " " + std::to_string(start.y)  + " "
        + std::to_string(end.x)    + " " + std::to_string(end.y)    + " "
        + std::to_string(static_cast<int>(algorithm)) + " "
        + std::to_string(color.r)  + " "
        + std::to_string(color.g)  + " "
        + std::to_string(color.b);
}
