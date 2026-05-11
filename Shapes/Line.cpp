#include "Line.h"
#include <string>

Line::Line() : Shape(), start(), end(), algorithm(LineAlgorithmType::DDA) {
    // TODO: Add initialization logic if needed.
}

Line::Line(const Point& start, const Point& end, LineAlgorithmType algorithm, const Color& color)
    : Shape(color), start(start), end(end), algorithm(algorithm) {
    // TODO: Add initialization logic if needed.
}

void Line::draw(HDC hdc) {
    // TODO: Call selected line drawing algorithm using the provided WinAPI HDC.
}

// Returns a text representation of the line for saving to a file.
std::string Line::serialize() const {
    // Implement serialization logic for the line.
    return "LINE"
        + std::to_string(start.x)  + " " + std::to_string(start.y)  + " "
        + std::to_string(end.x)    + " " + std::to_string(end.y)    + " "
        + std::to_string(static_cast<int>(algorithm)) + " "
        + std::to_string(color.r)  + " "
        + std::to_string(color.g)  + " "
        + std::to_string(color.b);
}
