#include "FillAlgorithms.h"
#include <algorithm>

FillAlgorithms::FillAlgorithms() {
    // TODO: Add initialization logic if needed.
}

namespace {
COLORREF toColorRef(const Color& color) {
    return RGB(color.r, color.g, color.b);
}

Color lerpColor(const Color& startColor, const Color& endColor, double t) {
    return Color(
        static_cast<int>(startColor.r + (endColor.r - startColor.r) * t),
        static_cast<int>(startColor.g + (endColor.g - startColor.g) * t),
        static_cast<int>(startColor.b + (endColor.b - startColor.b) * t));
}

void drawThickPixel(HDC hdc, int x, int y, COLORREF color) {
    SetPixel(hdc, x, y, color);
    SetPixel(hdc, x + 1, y, color);
    SetPixel(hdc, x, y + 1, color);
    SetPixel(hdc, x + 1, y + 1, color);
}

void drawQuarterPoints(HDC hdc, const Point& center, int x, int y, int quarter, COLORREF color) {
    switch (quarter) {
    case 1:
        drawThickPixel(hdc, center.x + x, center.y - y, color);
        drawThickPixel(hdc, center.x + y, center.y - x, color);
        break;
    case 2:
        drawThickPixel(hdc, center.x - x, center.y - y, color);
        drawThickPixel(hdc, center.x - y, center.y - x, color);
        break;
    case 3:
        drawThickPixel(hdc, center.x - x, center.y + y, color);
        drawThickPixel(hdc, center.x - y, center.y + x, color);
        break;
    case 4:
        drawThickPixel(hdc, center.x + x, center.y + y, color);
        drawThickPixel(hdc, center.x + y, center.y + x, color);
        break;
    default:
        drawThickPixel(hdc, center.x + x, center.y + y, color);
        drawThickPixel(hdc, center.x - x, center.y + y, color);
        drawThickPixel(hdc, center.x + x, center.y - y, color);
        drawThickPixel(hdc, center.x - x, center.y - y, color);
        drawThickPixel(hdc, center.x + y, center.y + x, color);
        drawThickPixel(hdc, center.x - y, center.y + x, color);
        drawThickPixel(hdc, center.x + y, center.y - x, color);
        drawThickPixel(hdc, center.x - y, center.y - x, color);
        break;
    }
}

void drawBresenhamCircle(HDC hdc, const Point& center, int radius, int quarter, COLORREF color) {
    int x = 0;
    int y = radius;
    int decision = 1 - radius;
    int changeEast = 3;
    int changeSouthEast = 5 - 2 * radius;

    drawQuarterPoints(hdc, center, x, y, quarter, color);

    while (x < y) {
        if (decision < 0) {
            decision += changeEast;
            changeSouthEast += 2;
        } else {
            decision += changeSouthEast;
            changeSouthEast += 4;
            --y;
        }

        changeEast += 2;
        ++x;

        drawQuarterPoints(hdc, center, x, y, quarter, color);
    }
}
}

void FillAlgorithms::floodFillRecursive(HDC hdc, const Point& start, const Color& fillColor) {
    // TODO: Implement recursive flood fill.
}

void FillAlgorithms::floodFillNonRecursive(HDC hdc, const Point& start, const Color& fillColor) {
    // TODO: Implement non-recursive flood fill.
}

void FillAlgorithms::fillCircle(HDC hdc, const Point& center, int radius, const Color& fillColor) {
    fillCircleWithCircles(hdc, center, 0, radius, 0, fillColor, fillColor);
}

void FillAlgorithms::fillCircleWithCircles(
    HDC hdc,
    const Point& center,
    int innerRadius,
    int outerRadius,
    int quarter,
    const Color& startColor,
    const Color& endColor) {
    if (!hdc) {
        return;
    }

    int minRadius = std::max(0, std::min(innerRadius, outerRadius));
    int maxRadius = std::max(innerRadius, outerRadius);

    if (maxRadius < 0) {
        return;
    }

    if (minRadius == maxRadius) {
        drawBresenhamCircle(hdc, center, maxRadius, quarter, toColorRef(endColor));
        return;
    }

    for (int radius = minRadius; radius <= maxRadius; ++radius) {
        double t = static_cast<double>(radius - minRadius) / (maxRadius - minRadius);
        Color color = lerpColor(startColor, endColor, t);
        drawBresenhamCircle(hdc, center, radius, quarter, toColorRef(color));
    }
}

void FillAlgorithms::fillRectangleWithCurves(HDC hdc, const Point& topLeft, const Point& bottomRight, const Color& fillColor) {
    // TODO: Implement rectangle filling with curves.
}

void FillAlgorithms::fillSquareWithCurves(HDC hdc, const Point& topLeft, int sideLength, const Color& fillColor) {
    // TODO: Implement square filling with curves.
}
