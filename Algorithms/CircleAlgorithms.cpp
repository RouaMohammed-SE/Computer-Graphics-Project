#include "CircleAlgorithms.h"
#include <windows.h>
#include <cmath>

using namespace std;

static void draw8Points(HDC hdc, int xc, int yc, int x, int y, COLORREF color) {
    SetPixel(hdc, xc+x, yc+y, color);
    SetPixel(hdc, xc-x, yc+y, color);
    SetPixel(hdc, xc-x, yc-y, color);
    SetPixel(hdc, xc+x, yc-y, color);
    SetPixel(hdc, xc+y, yc+x, color);
    SetPixel(hdc, xc-y, yc+x, color);
    SetPixel(hdc, xc-y, yc-x, color);
    SetPixel(hdc, xc+y, yc-x, color);
}

CircleAlgorithms::CircleAlgorithms() {
    // TODO: Add initialization logic if needed.
}

void CircleAlgorithms::drawDirect(HDC hdc, const Point& center, int radius, COLORREF color) {
    int xc = center.x, yc = center.y;
    int x = 0, y = radius;
    draw8Points(hdc, xc, yc, x, y, color);
    while (x < y) {
        x++;
        y = sqrt((double) radius*radius - x*x);
        draw8Points(hdc, xc, yc, x, y, color);
    }
}

void CircleAlgorithms::drawPolar(HDC hdc, const Point& center, int radius, COLORREF color) {
    int xc = center.x, yc = center.y;
    double dtheta = 1.0/radius, theta = 0;
    double x = radius, y = 0;
    draw8Points(hdc, xc, yc, x, y, color);
    while (x > y) {
        theta += dtheta;
        x = radius * cos(theta);
        y = radius * sin(theta);
        draw8Points(hdc, xc, yc, round(x), round(y), color);
    }
}

void CircleAlgorithms::drawIterativePolar(HDC hdc, const Point& center, int radius, COLORREF color) {
    int xc = center.x, yc = center.y;
    double dtheta = 1.0/radius, costheta = cos(dtheta), sintheta = sin(dtheta);
    double x = radius, y = 0;
    draw8Points(hdc, xc, yc, radius, 0, color);
    while (x > y) {
        double x2 = x * costheta - y * sintheta;
        y = x * sintheta + y * costheta;
        x = x2;
        draw8Points(hdc, xc, yc, round(x), round(y), color);
    }
}

void CircleAlgorithms::drawMidpoint(HDC hdc, const Point& center, int radius, COLORREF color) {
	int xc = center.x, yc = center.y;
    int x = 0, y = radius, d = 1 - radius;
    draw8Points(hdc, xc, yc, x, y, color);
    while (x < y) {
        if (d < 0) {
            d += 2*x + 3;
            x++;
        }
        else {
            d += 2 * (x - y) + 5;
            x++, y--;
        }
        draw8Points(hdc, xc, yc, x, y, color);
    }
}

void CircleAlgorithms::drawModifiedMidpoint(HDC hdc, const Point& center, int radius, COLORREF color) {
	int xc = center.x, yc = center.y;
	int x = 0, y = radius, d = 1 - radius, d1 = 3, d2 = 5 - 2 * radius;
    draw8Points(hdc, xc, yc, x, y, color);
    while (x < y) {
        if (d < 0) {
            d += d1;
            d1 += 2, d2 += 2;
            x++;
        }
        else {
            d += d2;
            d2 += 4, d1 += 2;
            x++, y--;
        }
        draw8Points(hdc, xc, yc, x, y, color);
    }
}
