#include "LineAlgorithms.h"
#include <windows.h>
#include <algorithm>
#include <cmath>

using namespace std;

LineAlgorithms::LineAlgorithms() {
    // TODO: Add initialization logic if needed.
}

void LineAlgorithms::drawDDA(HDC hdc, const Point& start, const Point& end, COLORREF color) {
    int x1 = start.x, y1 = start.y, x2 = end.x, y2 = end.y;
    if (x1 > x2) {
        swap(x1, x2);
        swap(y1, y2);
    }
    int dx = x2 - x1, dy = y2 - y1;
    if (abs(dx) > abs(dy)) {
        int x = x1;
        double y = y1, m = (double) dy / dx;
        SetPixel(hdc, x, y, color);
        while (x <= x2) {
            x++;
            y += m;
            SetPixel(hdc, x, round(y), color);
        }
    }
    else {
        if (y1 > y2) {
            swap(x1, x2);
            swap(y1, y2);
        }
        double x = x1, m = (double) dx / dy;
        int y = y1;
        SetPixel(hdc, x, y, color);
        while (y <= y2) {
            y++;
            x += m;
            SetPixel(hdc, round(x), y, color);
        }
    }
}

void LineAlgorithms::drawMidpoint(HDC hdc, const Point& start, const Point& end, COLORREF color) {
    int x1 = start.x, y1 = start.y, x2 = end.x, y2 = end.y;
    if (x1 > x2) {
        swap(x1, x2);
        swap(y1, y2);
    }
    int x, y, dx = abs(x2 - x1), dy = abs(y2 - y1), d, change1, change2;
    if (abs(dx) >= abs(dy)) {
        x = x1, y = y1, d = dx - 2*dy, change1 = -2*dy, change2 = 2*(dx - dy);
        int cy = y1 < y2 ? 1 : -1;
        SetPixel(hdc, x, y, color);
        while (x < x2) {
            if (d >= 0) x++, d += change1;
            else x++, y += cy, d += change2;
            SetPixel(hdc, x, y, color);
        }
    }
    else {
        if (y1 > y2) {
            swap(x1, x2);
            swap(y1, y2);
        }
        x = x1, y = y1, d = dy - 2*dx, change1 = -2*dx, change2 = 2*(dy - dx);
        int cx = x1 < x2 ? 1 : -1;
        SetPixel(hdc, x, y, color);
        while (y < y2) {
            if (d >= 0) y++, d += change1;
            else y++, x += cx, d += change2;
            SetPixel(hdc, x, y, color);
        }
    }
}

void LineAlgorithms::drawParametric(HDC hdc, const Point& start, const Point& end, COLORREF color) {
    for (double t = 0; t < 1; t += 0.0001) {
        double x = (end.x - start.x) * t + start.x;
        double y = (end.y - start.y) * t + start.y;
        SetPixel(hdc, round(x), round(y), color);
    }
}
