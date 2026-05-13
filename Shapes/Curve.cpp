#include "Curve.h"
#include "../Algorithms/CurveAlgorithms.h"
#include <string>

namespace {
COLORREF toColorRef(const Color& color) {
    return RGB(color.r, color.g, color.b);
}
}
Curve::Curve() : Shape(), controlPoints(), tension(0.0) {
    // TODO: Add initialization logic if needed.
}

Curve::Curve(const std::vector<Point>& controlPoints, double tension, const Color& color)
    : Shape(color), controlPoints(controlPoints), tension(tension) {
    // TODO: Add initialization logic if needed.
}

void Curve::draw(HDC hdc) {
    CurveAlgorithms::drawCardinalSpline(hdc, controlPoints, tension, toColorRef(color));
}

// Returns a text representation of the curve for saving to a file. 
std::string Curve::serialize() const {
    std::string result = "CURVE " + std::to_string(tension)
        + " " + std::to_string(controlPoints.size());
    for (const Point& p : controlPoints) {
        result += " " + std::to_string(p.x) + " " + std::to_string(p.y);
    }
    result += " " + std::to_string(color.r)
            + " " + std::to_string(color.g)
            + " " + std::to_string(color.b);
    return result;
}
