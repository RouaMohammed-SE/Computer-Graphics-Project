#include "Curve.h"

Curve::Curve() : Shape(), controlPoints(), tension(0.0) {
    // TODO: Add initialization logic if needed.
}

Curve::Curve(const std::vector<Point>& controlPoints, double tension, const Color& color)
    : Shape(color), controlPoints(controlPoints), tension(tension) {
    // TODO: Add initialization logic if needed.
}

void Curve::draw() {
    // TODO: Call curve drawing algorithm.
}
