#include "EllipseAlgorithms.h"
#include <windows.h>
#include <cmath>
#include <algorithm>

using namespace std;

EllipseAlgorithms::EllipseAlgorithms() {
    // TODO: Add initialization logic if needed.
}

void draw4Points(HDC hdc, int xc, int yc, int x, int y, COLORREF c) {
    SetPixel(hdc, xc+x, yc+y, c);
    SetPixel(hdc, xc-x, yc+y, c);
    SetPixel(hdc, xc-x, yc-y, c);
    SetPixel(hdc, xc+x, yc-y, c);
}

void EllipseAlgorithms::drawDirect(HDC hdc, const Point& center, int radiusX, int radiusY, COLORREF color) {
    int xc = center.x, yc = center.y;
    double x = radiusX, y = 0;
    draw4Points(hdc, xc, yc, x, y, c);
    while (x > y) { // abs(slope) > 1 --> y independent
        y++;
        x = (double) radiusX * sqrt(1.0 - (double) (y*y) / (radiusY*radiusY));
        draw4Points(hdc, xc, yc, round(x), y, c);
    }
    while (y >= x) {
        x--;
        y = (double) radiusY * sqrt(1.0 - (double) (x*x) / (radiusX*radiusX));
        draw4Points(hdc, xc, yc, round(x), y, c);
    }
}

void EllipseAlgorithms::drawPolar(HDC hdc, const Point& center, int radiusX, int radiusY, COLORREF color) {
    int xc = center.x, yc = center.y;
    double dtheta = 1.0/max(radiusX, radiusY), theta = 0;
    double x = radiusX, y = 0;
    draw4Points(hdc, xc+x, yc+y, x, y, c);
    while (theta < 1.57) {
        theta += dtheta;
        x = radiusX * cos(theta);
        y = radiusY * sin(theta);
        draw4Points(hdc, xc, yc, round(x), round(y), c);
    }
}

void EllipseAlgorithms::drawIterativePolar(HDC hdc, const Point& center, int radiusX, int radiusY, COLORREF color) {
    int xc = center.x, yc = center.y;
    int A = radiusX, B = radiusY;
    double dtheta = 1.0/max(A, B), costheta = cos(dtheta), sintheta = sin(dtheta);
    double x = A, y = 0, AoverB = (double) A/B, BoverA = (double) B/A, theta = 0;
    draw4Points(hdc, xc, yc, x, y, c);
    while (theta < 1.57) {
        double x2 = x * costheta - AoverB * y * sintheta;
        y = BoverA * x * sintheta + y * costheta;
        x = x2;
        draw4Points(hdc, xc, yc, round(x), round(y), c);
        theta += dtheta;
    }
}

void EllipseAlgorithms::drawMidpoint(HDC hdc, const Point& center, int radiusX, int radiusY, COLORREF color) {
    int xc = center.x, yc = center.y;
    int A = radiusX, B = radiusY;
    int x = 0, y = B;
    double d = B*B - A*A*B + A*A/4.0;
    draw4Points(hdc, xc, yc, round(x), round(y), c);
    while (B*B*x < A*A*y) {
        if (d < 0) {
            d += B*B * (2*x+3);
        }
        else {
            d += B*B * (2*x + 3) - 2*A*A*(y-1);
            y--;
        }
        x++;
        draw4Points(hdc, xc, yc, round(x), round(y), c);
    }
    d = B*B*(x+0.5)*(x+0.5) + A*A*(y-1)*(y-1) - A*A*B*B;
    while (y > 0) {
        if (d > 0) {
            d += A*A*(-2*y + 3);
        }
        else {
            d += 2*B*B * (x + 1) + A*A*(-2*y + 3);
            x++;
        }
        y--;
        draw4Points(hdc, xc, yc, round(x), round(y), c);
    }
}
