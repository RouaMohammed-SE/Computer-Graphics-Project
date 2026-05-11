#include "CurveAlgorithms.h"
#include <windows.h>

CurveAlgorithms::CurveAlgorithms() {
    // TODO: Add initialization logic if needed.
}

void hermiteCurve(HDC hdc, Point& p1, Point& s1, Point& p2, Point& s2, COLORREF color) {
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

void CurveAlgorithms::drawCardinalSpline(HDC hdc, const std::vector<Point>& points, double tension, COLORREF color) {
    int n = points.size();
    vector<Point> tangents(n);
    for (int i = 1; i < n-1; i++) {
        tangents[i] = {tension * (points[i+1].x - points[i-1].x), tension * (points[i+1].y - points[i-1].y)};
    }
    tangents[0] = {tension * (points[1].x - points[0].x), tension * (points[1].y - points[0].y)};
    tangents[n-1] = {tension * (points[n-1].x - points[n-2].x), tension * (points[n-1].y - points[n-2].y)};
    for (int i = 0; i < n-1; i++) {
        hermiteCurve(hdc, points[i], tangents[i], points[i+1], tangents[i+1], color);
    }
}
