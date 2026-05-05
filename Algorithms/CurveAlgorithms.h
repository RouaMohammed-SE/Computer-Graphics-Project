#ifndef ALGORITHMS_CURVE_ALGORITHMS_H
#define ALGORITHMS_CURVE_ALGORITHMS_H

#include "../Utils/Point.h"
#include <vector>

// Groups curve drawing algorithm entry points.
class CurveAlgorithms {
public:
    CurveAlgorithms();

    static void drawCardinalSpline(const std::vector<Point>& controlPoints, double tension);
};

#endif
