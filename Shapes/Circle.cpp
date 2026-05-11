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
