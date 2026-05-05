#include "Line.h"

Line::Line() : Shape(), start(), end(), algorithm(LineAlgorithmType::DDA) {
    // TODO: Add initialization logic if needed.
}

Line::Line(const Point& start, const Point& end, LineAlgorithmType algorithm, const Color& color)
    : Shape(color), start(start), end(end), algorithm(algorithm) {
    // TODO: Add initialization logic if needed.
}

void Line::draw() {
    // TODO: Call selected line drawing algorithm.
}
