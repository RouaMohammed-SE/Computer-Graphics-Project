#include "FillAlgorithms.h"
#include "../Algorithms/LineAlgorithms.h"
#include "../Algorithms/CurveAlgorithms.h"
#include <algorithm>
#include <cmath>
#include <queue>
#include <list>
#define MAXENTRIES 600

using namespace std;

//------------- Helpers definition ---------------
namespace {
    COLORREF toColorRef(const Color& color);

    Color lerpColor(const Color& startColor, const Color& endColor, double t);

    void drawThickPixel(HDC hdc, int x, int y, COLORREF color);

    void drawQuarterPoints(HDC hdc, const Point& center, int x, int y, int quarter, COLORREF color);

    void drawBresenhamCircle(HDC hdc, const Point& center, int radius, int quarter, COLORREF color);

    void drawQuarterLines(HDC hdc, const Point& center, int x, int y, int quarter, COLORREF color);

    struct Entry
    {
        int xleft,xright;
    };

    void InitEntries(Entry table[]);

    void ScanEdge(Point p1,Point p2,Entry table[]);

    void DrawScanLines(HDC hdc,Entry table[],COLORREF color);

    struct EdgeRec
    {
        double x;
        double minv;
        int ymax;
        bool operator<(EdgeRec r)
        {
            return x < r.x;
        }
    };

    typedef list<EdgeRec> EdgeList;

    EdgeRec InitEdgeRec(Point& p1,Point& p2);

    void InitEdgeTable(Point *polygon, int n, EdgeList table[]);
}

// -----------------------------------------------

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
    int x = start.x , y = start.y;
    COLORREF startColor = GetPixel(hdc, x, y);
    COLORREF fillRef = toColorRef(fillColor);
    COLORREF borderRef = toColorRef(borderColor);

    if (startColor == borderRef || startColor == fillRef) return;

    queue<Point>q;
    q.push({x,y});

    while(!q.empty()){
        Point p = q.front();
        q.pop();

        COLORREF c = GetPixel(hdc , p.x , p.y);

        if(c == borderRef || c == fillRef) continue;
        SetPixel(hdc , p.x , p.y , fillRef);
        q.push({p.x-1,p.y});
        q.push({p.x+1,p.y});
        q.push({p.x,p.y-1});
        q.push({p.x,p.y+1});
    }
}

void FillAlgorithms::fillCircle(HDC hdc, const Point& center, int radius, const Color& fillColor) {
    fillCircleWithCircles(hdc, center, 0, radius, 0, fillColor, fillColor);
}

void FillAlgorithms::fillCircleWithLines(HDC hdc, const Point& center, int radius, int quarter, const Color& fillColor) {
    if (!hdc || radius < 0) {
        return;
    }

    COLORREF color = toColorRef(fillColor);
    int x = 0;
    int y = radius;
    int decision = 1 - radius;
    int changeEast = 3;
    int changeSouthEast = 5 - 2 * radius;

    drawQuarterLines(hdc, center, x, y, quarter, color);

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
        drawQuarterLines(hdc, center, x, y, quarter, color);
    }
}

void FillAlgorithms::fillCircleWithCircles(HDC hdc, const Point& center, int innerRadius, int outerRadius, int quarter, const Color& startColor, const Color& endColor) {
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

        CurveAlgorithms::bezierCurve(hdc, p1, p2, p3, p4, fillColor);
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

        CurveAlgorithms::hermiteCurve(hdc, p1, s1, p2, s2, fillColor);
    }
}

void FillAlgorithms::convexFill(HDC hdc,vector<Point> p,int n,COLORREF color){
    Entry *table = new Entry[MAXENTRIES];

    InitEntries(table);

    Point p1 = p[n-1];
    for(int i = 0; i < n; i++){
        Point p2 = p[i];
        ScanEdge(p1,p2,table);
        p1 = p[i];
    }
    DrawScanLines(hdc, table, color);
    delete [] table;
}

void FillAlgorithms::nonConvexFill(HDC hdc, vector<Point> p, int n, COLORREF c){
    auto *table = new EdgeList [MAXENTRIES];
    Point *polygon = p.data();
    InitEdgeTable(polygon, n, table);

    int y = 0;
    while(y < MAXENTRIES && table[y].empty())
        y++;

    if(y == MAXENTRIES) return;

    EdgeList ActiveList = table[y];

    while (!ActiveList.empty()){
        ActiveList.sort();

        for(auto it=ActiveList.begin(); it!=ActiveList.end(); ++it){
            int x1 = (int)ceil(it->x);
            it++;
            int x2 = (int)floor(it->x);
            for(int x = x1; x <= x2; x++)
                SetPixel(hdc,x,y,c);
        }
        y++;

        auto it = ActiveList.begin();
        while(it != ActiveList.end()) {
            if(y == it->ymax) it = ActiveList.erase(it);
            else it++;
        }

        for(auto & it1 : ActiveList)
            it1.x += it1.minv;

        if (y < MAXENTRIES)
            ActiveList.insert(ActiveList.end(),table[y].begin(),table[y].end());
    }
    delete[] table;
}

//------------- Helpers Implementation ---------------

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

    void drawQuarterLines(HDC hdc, const Point& center, int x, int y, int quarter, COLORREF color) {
        switch (quarter) {
            case 1:
                LineAlgorithms::drawDDA(hdc, center, Point(center.x + x, center.y - y), color);
                LineAlgorithms::drawDDA(hdc, center, Point(center.x + y, center.y - x), color);
                break;
            case 2:
                LineAlgorithms::drawDDA(hdc, center, Point(center.x - x, center.y - y), color);
                LineAlgorithms::drawDDA(hdc, center, Point(center.x - y, center.y - x), color);
                break;
            case 3:
                LineAlgorithms::drawDDA(hdc, center, Point(center.x - x, center.y + y), color);
                LineAlgorithms::drawDDA(hdc, center, Point(center.x - y, center.y + x), color);
                break;
            case 4:
                LineAlgorithms::drawDDA(hdc, center, Point(center.x + x, center.y + y), color);
                LineAlgorithms::drawDDA(hdc, center, Point(center.x + y, center.y + x), color);
                break;
            default:
                LineAlgorithms::drawDDA(hdc, center, Point(center.x + x, center.y + y), color);
                LineAlgorithms::drawDDA(hdc, center, Point(center.x - x, center.y + y), color);
                LineAlgorithms::drawDDA(hdc, center, Point(center.x + x, center.y - y), color);
                LineAlgorithms::drawDDA(hdc, center, Point(center.x - x, center.y - y), color);
                LineAlgorithms::drawDDA(hdc, center, Point(center.x + y, center.y + x), color);
                LineAlgorithms::drawDDA(hdc, center, Point(center.x - y, center.y + x), color);
                LineAlgorithms::drawDDA(hdc, center, Point(center.x + y, center.y - x), color);
                LineAlgorithms::drawDDA(hdc, center, Point(center.x - y, center.y - x), color);
                break;
        }
    }

    void InitEntries(Entry table[]) {
        for (int i = 0; i < MAXENTRIES; i++) {
            table[i].xleft = INT_MAX;
            table[i].xright = INT_MIN;
        }
    }

    void ScanEdge(Point p1,Point p2,Entry table[])
    {
        if(p1.y == p2.y) return;
        if(p1.y > p2.y) swap(p1,p2);

        double minv = (double) (p2.x-p1.x) / (p2.y-p1.y);
        double x = p1.x;
        int y = p1.y;

        while(y < p2.y)
        {
            if(x < table[y].xleft) table[y].xleft = (int)ceil(x);
            if(x > table[y].xright) table[y].xright = (int)floor(x);
            y++;
            x+=minv;
        }
    }

    void DrawScanLines(HDC hdc,Entry table[],COLORREF color){
        for(int y = 0; y < MAXENTRIES; y++) {
            if(table[y].xleft < table[y].xright) {
                for(int x = table[y].xleft; x <= table[y].xright; x++)
                    SetPixel(hdc,x,y,color);
            }
        }
    }

    EdgeRec InitEdgeRec(Point& p1,Point& p2){
        if(p1.y > p2.y) swap(p1,p2);
        EdgeRec rec;
        rec.x = p1.x;
        rec.ymax = p2.y;
        rec.minv = (double)(p2.x-p1.x) / (p2.y-p1.y);
        return rec;
    }

    void InitEdgeTable(Point *polygon, int n, EdgeList table[]){
        Point p1 = polygon[n-1];
        for(int i = 0;i < n; i++){
            Point p2 = polygon[i];
            if(p1.y == p2.y) {
                p1 = p2;
                continue;
            }
            EdgeRec rec = InitEdgeRec(p1, p2);
            table[p1.y].push_back(rec);
            p1 = polygon[i];
        }
    }
}
