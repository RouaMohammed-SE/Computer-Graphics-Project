#include "Circle.h"

Circle::Circle() : Shape(), center(), radius(0), algorithm(CircleAlgorithmType::Midpoint) {
    // TODO: Add initialization logic if needed.
}

Circle::Circle(const Point& center, int radius, CircleAlgorithmType algorithm, const Color& color)
    : Shape(color), center(center), radius(radius), algorithm(algorithm) {
    // TODO: Add initialization logic if needed.
}

void Circle::draw(HDC hdc) {
    // TODO: Call selected circle drawing algorithm using the provided WinAPI HDC.
}

// Returns a text representation of the circle for saving to a file.
std::string Circle::serialize() const {
    return "CIRCLE "
        + std::to_string(center.x) + " " + std::to_string(center.y) + " "
        + std::to_string(radius)   + " "
        + std::to_string(static_cast<int>(algorithm)) + " "
        + std::to_string(color.r)  + " "
        + std::to_string(color.g)  + " "
        + std::to_string(color.b);
}
