#include "Clipper.h"
#include <windows.h>
#include <cmath>
#include <algorithm>

using namespace std;

//------------- Helpers definition ---------------

double xl,xr,yb,yt; // four edges of the window

union outcode{
    struct{unsigned L:1,R:1,B:1,T:1;};
    unsigned all:4;
};

outcode getCode(Point p1);
void intersect(Point& p, Point p1, Point p2, int edge);
bool cohensuth(Point& p1, Point& p2);

//-------------------------------------

Clipper::Clipper() {
    // TODO: Add initialization logic if needed.
}

void Clipper::quadPointClipping(HDC hdc, Point &topleft , Point &bottomright , Point& point , COLORREF c) {
    xl = min(topleft.x, bottomright.x);
    xr = max(topleft.x, bottomright.x);
    yb = min(topleft.y, bottomright.y);
    yt = max(topleft.y, bottomright.y);

    outcode code = getCode(point);
    if (code.all == 0) {
        SetPixel(hdc , point.x , point.y , c);
    }
}

void Clipper::quadLineClipping(HDC hdc, Point &topleft , Point &bottomright , Point& start , Point& end , COLORREF c) {

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

//------------- Helpers Algorithms ---------------

outcode getCode(Point p1){
    outcode ans;
    ans.all = 0;

    if(p1.x < xl) ans.L = 1;
    if(p1.x > xr) ans.R = 1;
    if(p1.y < yb) ans.B = 1;
    if(p1.y > yt) ans.T = 1;

    return ans;
}

void intersect(Point& p, Point p1, Point p2, int edge) {
    double x, y;

    if (edge == 0) { // Left
        x = xl;
        y = p1.y + (xl - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
    }
    else if (edge == 1) { // Right
        x = xr;
        y = p1.y + (xr - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
    }
    else if (edge == 2) { // Bottom
        y = yb;
        x = p1.x + (yb - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
    }
    else { // Top
        y = yt;
        x = p1.x + (yt - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
    }

    p.x = x;
    p.y = y;
}

bool cohensuth(Point& p1, Point& p2) {
    while (true) {
        outcode c1 = getCode(p1);
        outcode c2 = getCode(p2);

        if (c1.all == 0 && c2.all == 0)
            return true;

        if ((c1.all & c2.all) != 0)
            return false;

        int edge;
        if (c1.all != 0) {
            if (c1.L) edge = 0;
            else if (c1.R) edge = 1;
            else if (c1.B) edge = 2;
            else edge = 3;

            intersect(p1, p1, p2, edge);
        }
        else {
            if (c2.L) edge = 0;
            else if (c2.R) edge = 1;
            else if (c2.B) edge = 2;
            else edge = 3;

            intersect(p2, p1, p2, edge);
        }
    }
}