#include "Clipper.h"
#include <windows.h>
#include <cmath>

Clipper::Clipper() {
    // TODO: Add initialization logic if needed.
}

void Clipper::clipAgainstRectangle(Shape* shape, const Point& topLeft, const Point& bottomRight) {
    // TODO: Implement rectangle clipping.
}

void Clipper::clipAgainstSquare(Shape* shape, const Point& topLeft, int sideLength) {
    // TODO: Implement square clipping.
}

void Clipper::clipAgainstCircle(Shape* shape, const Point& center, int radius) {
    // TODO: Implement optional circle clipping.
}

void Clipper::circlePointClipping(HDC hdc, Point &center, int radius, Point &point, COLORREF c) {
    int dx = point.x - center.x;
    int dy = point.y - center.y;
    if ((dx * dx + dy * dy) <= (radius * radius)) {
        SetPixel(hdc, point.x, point.y, c);
    }
}

void Clipper::circleLineClipping(HDC hdc, Point &center, int radius, Point &point1, Point &point2, COLORREF c) {
    Point p1, p2, start(-1,-1), end(-1, -1);
    p1.x = point1.x, p1.y = point1.y;
    p2.x = point2.x, p2.y = point2.y;

    if (p1.x > p2.x) {
        if (p1.x > p2.x) {
            Point temp = p1;
            p1 = p2;
            p2 = temp;
        }
    }

    int dx = p1.x - p2.x, dy = p1.y - p2.y;
    if (abs(dx) > abs(dy)) {
        int x = p1.x;
        double y = p1.y, m = (double) dy / dx;
        while (x <= p2.x) {
            int dx1 = x - center.x, dy1 = round(y) - center.y;
            if ((dx1 * dx1 + dy1 * dy1) <= (radius * radius)) {
                SetPixel(hdc, x, round(y), c);
            }
            x++;
            y += m;
        }
    }
    else {
        double x = p1.x, m = (double) dx / dy;
        int y = p1.y;
        if (p1.y < p2.y) {
            while (y < p2.y) {
                int dx1 = x - center.x, dy1 = round(y) - center.y;
                if ((dx1 * dx1 + dy1 * dy1) <= (radius * radius)) {
                    SetPixel(hdc, round(x), y, c);
                }
                y++;
                x += m;
            }
        }
        else {
            while (y > p2.y) {
                int dx1 = x - center.x, dy1 = round(y) - center.y;
                if ((dx1 * dx1 + dy1 * dy1) <= (radius * radius)) {
                    SetPixel(hdc, round(x), y, c);
                }
                y--;
                x += m;
            }
        }
    }
}
