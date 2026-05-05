#ifndef ALGORITHMS_LINE_ALGORITHMS_H
#define ALGORITHMS_LINE_ALGORITHMS_H

#include "../Utils/Point.h"

// Groups line drawing algorithm entry points.
class LineAlgorithms {
public:
    LineAlgorithms();

    static void drawDDA(const Point& start, const Point& end);
    static void drawMidpoint(const Point& start, const Point& end);
    static void drawParametric(const Point& start, const Point& end);
};

#endif
