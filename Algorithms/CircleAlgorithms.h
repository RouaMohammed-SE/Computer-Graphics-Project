#ifndef ALGORITHMS_CIRCLE_ALGORITHMS_H
#define ALGORITHMS_CIRCLE_ALGORITHMS_H

#include "../Utils/Point.h"

// Groups circle drawing algorithm entry points.
class CircleAlgorithms {
public:
    CircleAlgorithms();

    static void drawDirect(const Point& center, int radius);
    static void drawPolar(const Point& center, int radius);
    static void drawIterativePolar(const Point& center, int radius);
    static void drawMidpoint(const Point& center, int radius);
    static void drawModifiedMidpoint(const Point& center, int radius);
};

#endif
