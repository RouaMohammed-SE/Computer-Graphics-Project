#include "FillAlgorithms.h"
#include <algorithm>
#include <cmath>

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

void FillAlgorithms::floodFillRecursive(HDC hdc, const Point& start, const Color& fillColor , const Color& borderColor) {
    int x = start.x , y = start.y;
    COLORREF c = GetPixel(hdc , x , y);
    COLORREF fillRef  = toColorRef(fillColor);
    COLORREF borderRef = toColorRef(borderColor);
    if(c != fillRef && c != borderRef){
        SetPixel(hdc , x , y , fillRef);
        floodFillRecursive(hdc , {x+1,y} , fillColor , borderColor);
        floodFillRecursive(hdc , {x-1,y} , fillColor , borderColor);
        floodFillRecursive(hdc , {x , y+1} , fillColor , borderColor);
        floodFillRecursive(hdc , {x , y-1 }, fillColor , borderColor);
    }
}

void FillAlgorithms::floodFillNonRecursive(HDC hdc, const Point& start, const Color& fillColor , const Color& borderColor) {
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

void bezierCurve(HDC hdc, Point& p1, Point& p2, Point& p3, Point& p4, COLORREF color) {
    for (double t = 0; t < 1; t += 0.0001) {
        double tt = (1-t);
        double a0 = tt*tt*tt, a1 = tt*tt*3*t, a2 = tt*3*t*t, a3 = t*t*t;
        int x = round(a0*p1.x + a1*p2.x + a2*p3.x + a3*p4.x);
        int y = round(a0*p1.y + a1*p2.y + a2*p3.y + a3*p4.y);
        SetPixel(hdc, x, y, color);
    }
}

void FillAlgorithms::fillRectangleWithCurves(HDC hdc, const Point& topLeft, const Point& bottomRight, COLORREF fillColor) {
    int left = topLeft.x, top = topLeft.y;
    int right = bottomRight.x, bottom = bottomRight.y;
    int width = right - left;
    double coeff = width * 0.4;

    for (int i = 0; i <= 80; i++) {
        double t = (double) i / 80;

        double y = top + t * (bottom - top);
        Point p1(left, y), p4(right, y);

        double wave = sin(t * 3.14) * coeff;
        Point p2(left  + width/3.0,  y - wave), p3(right - width/3.0,  y + wave);

        bezierCurve(hdc, p1, p2, p3, p4, fillColor);
    }
}

void hermiteCurve1(HDC hdc, Point& p1, Point& s1, Point& p2, Point& s2, COLORREF color) {
    int H[][4] = {{2, 1, -2, 1}, {-3, -2, 3, -1}, {0, 1, 0, 0}, {1, 0, 0, 0}};
    int a3 = H[0][0]*p1.x + H[0][1]*s1.x + H[0][2]*p2.x + H[0][3]*s2.x;
    int a2 = H[1][0]*p1.x + H[1][1]*s1.x + H[1][2]*p2.x + H[1][3]*s2.x;
    int a1 = H[2][0]*p1.x + H[2][1]*s1.x + H[2][2]*p2.x + H[2][3]*s2.x;
    int a0 = H[3][0]*p1.x + H[3][1]*s1.x + H[3][2]*p2.x + H[3][3]*s2.x;

    int b3 = H[0][0]*p1.y + H[0][1]*s1.y + H[0][2]*p2.y + H[0][3]*s2.y;
    int b2 = H[1][0]*p1.y + H[1][1]*s1.y + H[1][2]*p2.y + H[1][3]*s2.y;
    int b1 = H[2][0]*p1.y + H[2][1]*s1.y + H[2][2]*p2.y + H[2][3]*s2.y;
    int b0 = H[3][0]*p1.y + H[3][1]*s1.y + H[3][2]*p2.y + H[3][3]*s2.y;

    for (double t = 0; t < 1; t += 0.0001) {
        double tsquare = t*t, tcube = tsquare*t;
        int x = round(a3*tcube + a2*tsquare + a1*t + a0);
        int y = round(b3*tcube + b2*tsquare + b1*t + b0);
        SetPixel(hdc, x, y, color);
    }
}

void FillAlgorithms::fillSquareWithCurves(HDC hdc, const Point& topLeft, int sideLength, COLORREF fillColor) {
    int left = topLeft.x, top = topLeft.y;
    int right = topLeft.x + sideLength, bottom = topLeft.y + sideLength;
    double tangentMag = sideLength * 0.8;

    for (int i = 0; i <= 80; i++) {
        double t = (double) i / 80;

        double x = left + t * (right - left);
        Point p1(x, top), p2(x, bottom);

        double wave = sin(t * 3.14) * tangentMag;
        Point s1(wave, tangentMag), s2(wave, tangentMag);

        hermiteCurve1(hdc, p1, s1, p2, s2, fillColor);
    }
}
