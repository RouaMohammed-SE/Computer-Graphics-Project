#include "Application.h"
#include "../Menu/MenuCommands.h"
#include "../Algorithms/LineAlgorithms.h"
#include "../Algorithms/CircleAlgorithms.h"
#include "../Algorithms/EllipseAlgorithms.h"
#include "../Algorithms/CurveAlgorithms.h"
#include "../Filling/FillAlgorithms.h"
#include "../Clipping/Clipper.h"
#include "../Core/Enums.h"
#include "../Shapes/Circle.h"
#include "../Shapes/Curve.h"
#include "../Shapes/Ellipse.h"
#include "../Shapes/Line.h"
#include <commdlg.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <commctrl.h>

using namespace std;

// ============================================================
//  Constructor / Destructor
// ============================================================
Application::Application()
    : window(1100, 700, Color(255, 255, 255)),
      inputHandler(),
      menu(),
      fileManager(),
      preferences(),
      logger(),
      shapes(),
      persistentDrawings(),
      pendingClicks(),
      fillQuarter(1),
      pendingHappyFace(true),
      drawingColor(0, 0, 0),
      floodFillingColor(255, 0, 0),
      backgroundColor(255, 255, 255) {
    window.setPaintCallback(Application::handlePaint, this);
    window.setMouseClickCallback(Application::handleMouseClick, this);
    window.setMouseMoveCallback(Application::handleMouseMove, this);
    window.setRightClickCallback(Application::handleRightClick, this);
    window.setCommandCallback(Application::handleCommand, this);
}

Application::~Application() {
    for (Shape* shape : shapes) delete shape;
    shapes.clear();
}

// ============================================================
//  run()  —  creates window, installs canvas subclass, message loop
// ============================================================
void Application::run() {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$",  "r", stdin);

    logger.log("Application started.");
    logger.log("Use the sidebar on the left to select drawing tools.");

    if (!window.create(L"Drawing App")) {
        logger.error("Failed to create window.");
        return;
    }

    // ------------------------------------------------------------------
    //  Subclass the canvas HWND so we receive mouse events there directly.
    //  (The canvas is a plain child window; its WndProc is DefWindowProc.)
    //  We use SetWindowSubclass so our proc only handles the messages we care
    //  about and forwards everything else.
    // ------------------------------------------------------------------
    HWND hCanvas = window.getHandle(); // getHandle() returns main HWND for now;
    // NOTE: If you expose getCanvasHandle() from Window, use that instead.
    // For now we rely on WM_APP messages forwarded from child windows.

    // Handle palette colour-pick message (posted by palette bar on swatch click)
    // We do this in the message loop below.

    // Update initial status
    window.setStatusText(L"Ready  ·  Select a tool from the sidebar");
    window.setModeText(L"No tool selected");

    // Run message loop — intercept WM_APP+1 (palette swatch click)
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_APP + 1 && msg.hwnd == window.getHandle()) {
            // Palette swatch clicked — wParam is the COLORREF
            COLORREF c = (COLORREF)msg.wParam;
            Color chosen(GetRValue(c), GetGValue(c), GetBValue(c));
            setDrawingColor(chosen);
            logger.log("Drawing colour changed via palette.");
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

// ============================================================
//  update / render
// ============================================================
void Application::update() {}

void Application::render(HDC hdc) {
    for (Shape* shape : shapes) shape->draw(hdc);
    for (const PersistentDrawing& drawing : persistentDrawings) replayPersistentDrawing(hdc, drawing);
    drawPendingClickMarkers(hdc);
}

void Application::addShape(Shape* shape) {
    if (shape) shapes.push_back(shape);
}

void Application::clearShapes() {
    for (Shape* shape : shapes) delete shape;
    shapes.clear();
    persistentDrawings.clear();
    logger.log("Cleared all shapes.");
}

void Application::setDrawingColor(const Color& color) {
    drawingColor = color;
    preferences.setDrawingColor(color);
}

void Application::setBackgroundColor(const Color& color) {
    backgroundColor = color;
    preferences.setBackgroundColor(color);
    window.setBackgroundColor(color);
}

void Application::resetPendingClicks() { pendingClicks.clear(); }

// ============================================================
//  Status/mode helpers
// ============================================================
void Application::updateStatusMode(const std::wstring& mode, const std::wstring& algo) {
    std::wstring modeStr = mode;
    if (!algo.empty()) modeStr += L"  \u00b7  " + algo;
    window.setModeText(modeStr);
    std::wstring status = L"Mode: " + mode;
    if (!algo.empty()) status += L"  \u2014  Algorithm: " + algo;
    window.setStatusText(status);
}

// ============================================================
//  Callbacks
// ============================================================
void Application::handlePaint(HDC hdc, void* context) {
    Application* app = static_cast<Application*>(context);
    app->render(hdc);
}

void Application::handleMouseClick(const Point& position, void* context) {
    Application* app = static_cast<Application*>(context);
    app->inputHandler.processMouseClick(position);

    // The canvas child window forwards WM_LBUTTONDOWN to the main HWND's
    // mouseClickCallback.  Position is already canvas-relative.
    HDC hdc = GetDC(app->window.getCanvasHandle());
    DrawingMode mode = app->menu.getMode();
    COLORREF color = RGB(app->drawingColor.r, app->drawingColor.g, app->drawingColor.b);

    if (mode == DrawingMode::DrawLine) {
        app->pendingClicks.push_back(position);
        if (app->pendingClicks.size() == 1) {
            app->logger.log("Line start set. Click line end.");
            app->window.setStatusText(L"Line: click the end point");
        } else if (app->pendingClicks.size() == 2) {
            app->addShape(new Line(app->pendingClicks[0], app->pendingClicks[1],
                                   app->menu.getLineAlgorithm(), app->drawingColor));
            app->resetPendingClicks();
            app->window.refresh();
            app->logger.log("Line drawn.");
            app->window.setStatusText(L"Line drawn. Click again for another.");
        }
    }
    else if (mode == DrawingMode::DrawCircle) {
        app->pendingClicks.push_back(position);
        if (app->pendingClicks.size() == 1) {
            app->logger.log("Circle center set. Click a boundary point.");
            app->window.setStatusText(L"Circle: click a boundary point");
        } else if (app->pendingClicks.size() == 2) {
            const Point& center = app->pendingClicks[0];
            const Point& boundary = app->pendingClicks[1];
            int dx = boundary.x - center.x, dy = boundary.y - center.y;
            int radius = static_cast<int>(std::round(std::sqrt(dx*dx + dy*dy)));
            app->addShape(new Circle(center, radius, app->menu.getCircleAlgorithm(), app->drawingColor));
            app->resetPendingClicks();
            app->window.refresh();
            app->logger.log("Circle drawn.");
            app->window.setStatusText(L"Circle drawn.");
        }
    }
    else if (mode == DrawingMode::DrawEllipse) {
        app->pendingClicks.push_back(position);
        if (app->pendingClicks.size() == 1) {
            app->logger.log("Ellipse center set. Click a point for horizontal radius.");
            app->window.setStatusText(L"Ellipse: click horizontal radius point");
        } else if (app->pendingClicks.size() == 2) {
            app->window.setStatusText(L"Ellipse: click vertical radius point");
        } else if (app->pendingClicks.size() == 3) {
            const Point& center = app->pendingClicks[0];
            int radiusX = std::abs(app->pendingClicks[1].x - center.x);
            int radiusY = std::abs(app->pendingClicks[2].y - center.y);
            app->addShape(new MyEllipse(center, radiusX, radiusY, app->menu.getEllipseAlgorithm(), app->drawingColor));
            app->resetPendingClicks();
            app->window.refresh();
            app->logger.log("Ellipse drawn.");
            app->window.setStatusText(L"Ellipse drawn.");
        }
    }
    else if (mode == DrawingMode::DrawCurve) {
        app->pendingClicks.push_back(position);
        app->logger.log("Curve control point set. Right-click to finalize.");
        app->window.setStatusText(L"Curve: add more points or right-click to finish");
    }
    else if (mode == DrawingMode::DrawSmiley) {
        app->addSmileyFace(position, app->pendingHappyFace);
    }
    else if (mode == DrawingMode::Fill) {
        FillAlgorithmType fillType = app->menu.getFillAlgorithm();

        if (fillType == FillAlgorithmType::CircleFillWithLines) {
            app->pendingClicks.push_back(position);
            if (app->pendingClicks.size() == 1) {
                app->logger.log("Fill Circle: center set. Click boundary.");
                app->window.setStatusText(L"Fill Circle w/ Lines: click boundary point");
            } else if (app->pendingClicks.size() == 2) {
                const Point& center = app->pendingClicks[0];
                int dx = position.x - center.x, dy = position.y - center.y;
                int radius = static_cast<int>(std::round(std::sqrt(dx*dx + dy*dy)));
                CircleAlgorithms::drawMidpoint(hdc, center, radius, color);
                FillAlgorithms::fillCircleWithLines(hdc, center, radius, app->fillQuarter, app->drawingColor);
                app->persistentDrawings.push_back({PersistentDrawingType::CircleFillWithLines,
                    {center}, radius, 0, 0, app->fillQuarter, 0, app->drawingColor});
                app->resetPendingClicks();
                app->window.setStatusText(L"Fill Circle with Lines done.");
                app->logger.log("Fill Circle with lines done.");
            }
        }
        else if (fillType == FillAlgorithmType::CircleFillWithCircles) {
            app->pendingClicks.push_back(position);
            if (app->pendingClicks.size() == 1) {
                app->window.setStatusText(L"Fill Circle w/ Circles: click inner radius");
            } else if (app->pendingClicks.size() == 2) {
                app->window.setStatusText(L"Fill Circle w/ Circles: click outer radius");
            } else if (app->pendingClicks.size() == 3) {
                const Point& center = app->pendingClicks[0];
                auto dist = [&](const Point& a, const Point& b) {
                    int dx = a.x-b.x, dy = a.y-b.y;
                    return static_cast<int>(std::round(std::sqrt(dx*dx+dy*dy)));
                };
                int innerRadius = dist(center, app->pendingClicks[1]);
                int outerRadius = dist(center, app->pendingClicks[2]);
                CircleAlgorithms::drawMidpoint(hdc, center, outerRadius, color);
                FillAlgorithms::fillCircleWithCircles(hdc, center, innerRadius, outerRadius,
                    app->fillQuarter, app->drawingColor, app->drawingColor);
                app->persistentDrawings.push_back({PersistentDrawingType::CircleFillWithCircles,
                    {center}, 0, innerRadius, outerRadius, app->fillQuarter, 0, app->drawingColor});
                app->resetPendingClicks();
                app->window.setStatusText(L"Fill Circle with Circles done.");
                app->logger.log("Fill Circle with circles done.");
            }
        }
        else if (fillType == FillAlgorithmType::SquareFillWithCurves) {
            static int clickCount = 0;
            static Point topLeft;
            if (clickCount == 0) { topLeft = position; clickCount = 1; app->window.setStatusText(L"Fill Square: click any point to set size"); }
            else {
                int side = (int)sqrt((double)(topLeft.x-position.x)*(topLeft.x-position.x) + (double)(topLeft.y-position.y)*(topLeft.y-position.y));
                FillAlgorithms::fillSquareWithCurves(hdc, topLeft, side, color);
                app->persistentDrawings.push_back({PersistentDrawingType::SquareFillWithCurves,
                    {topLeft}, 0,0,0,0, side, app->drawingColor});
                app->window.setStatusText(L"Fill Square with Hermite Curves done.");
                app->logger.log("Fill Square done.");
                clickCount = 0;
            }
        }
        else if (fillType == FillAlgorithmType::RectangleFillWithCurves) {
            static int clickCount = 0;
            static Point topLeft;
            if (clickCount == 0) { topLeft = position; clickCount = 1; app->window.setStatusText(L"Fill Rectangle: click bottom-right corner"); }
            else {
                FillAlgorithms::fillRectangleWithCurves(hdc, topLeft, position, color);
                app->persistentDrawings.push_back({PersistentDrawingType::RectangleFillWithCurves,
                    {topLeft, position}, 0,0,0,0,0, app->drawingColor});
                app->window.setStatusText(L"Fill Rectangle with Bezier Curves done.");
                app->logger.log("Fill Rectangle done.");
                clickCount = 0;
            }
        }
        else if (fillType == FillAlgorithmType::FloodFillRecursive) {
            FillAlgorithms::floodFillRecursive(hdc, position, app->floodFillingColor, app->drawingColor);
            PersistentDrawing pd;
            pd.type = PersistentDrawingType::FloodFillRecursive;
            pd.floodPoint = position; pd.floodColor = app->floodFillingColor; pd.borderColor = app->drawingColor;
            app->persistentDrawings.push_back(pd);
            app->window.setStatusText(L"Flood Fill (Recursive) done.");
            app->logger.log("Flood Fill (Recursive) done.");
        }
        else if (fillType == FillAlgorithmType::FloodFillNonRecursive) {
            FillAlgorithms::floodFillNonRecursive(hdc, position, app->floodFillingColor, app->drawingColor);
            PersistentDrawing pd;
            pd.type = PersistentDrawingType::FloodFillNonRecursive;
            pd.floodPoint = position; pd.floodColor = app->floodFillingColor; pd.borderColor = app->drawingColor;
            app->persistentDrawings.push_back(pd);
            app->window.setStatusText(L"Flood Fill (Non-Recursive) done.");
            app->logger.log("Flood Fill (Non-Recursive) done.");
        }
        else if (fillType == FillAlgorithmType::ConvexFill) {
            static std::vector<Point> convexPoints;
            convexPoints.push_back(position);
            SetPixel(hdc, position.x, position.y, color);
            if (convexPoints.size() > 1) LineAlgorithms::drawDDA(hdc, convexPoints[convexPoints.size()-2], position, color);
            app->window.setStatusText(L"Convex Fill: keep clicking. Double-click last point to finish.");
            int n = convexPoints.size();
            if (n >= 3 && convexPoints[n-1].x == convexPoints[n-2].x && convexPoints[n-1].y == convexPoints[n-2].y) {
                convexPoints.pop_back();
                FillAlgorithms::convexFill(hdc, convexPoints, (int)convexPoints.size(), color);
                PersistentDrawing pd; pd.type = PersistentDrawingType::ConvexPolygonFill;
                pd.polygonPoints = convexPoints; pd.color = app->drawingColor;
                app->persistentDrawings.push_back(pd);
                app->window.setStatusText(L"Convex Fill done.");
                app->logger.log("Convex Fill done.");
                convexPoints.clear();
            }
        }
        else if (fillType == FillAlgorithmType::NonConvexFill) {
            static std::vector<Point> ncPoints;
            ncPoints.push_back(position);
            SetPixel(hdc, position.x, position.y, color);
            if (ncPoints.size() > 1) LineAlgorithms::drawDDA(hdc, ncPoints[ncPoints.size()-2], position, color);
            app->window.setStatusText(L"Non-Convex Fill: keep clicking. Double-click last point to finish.");
            int n = ncPoints.size();
            if (n >= 3 && ncPoints[n-1].x == ncPoints[n-2].x && ncPoints[n-1].y == ncPoints[n-2].y) {
                ncPoints.pop_back();
                FillAlgorithms::nonConvexFill(hdc, ncPoints, (int)ncPoints.size(), color);
                PersistentDrawing pd; pd.type = PersistentDrawingType::NonConvexPolygonFill;
                pd.polygonPoints = ncPoints; pd.color = app->drawingColor;
                app->persistentDrawings.push_back(pd);
                app->window.setStatusText(L"Non-Convex Fill done.");
                app->logger.log("Non-Convex Fill done.");
                ncPoints.clear();
            }
        }
    }
    else if (mode == DrawingMode::Clip) {
        // (Clipping logic unchanged — same as original Application.cpp)
        static Clipper clipper;
        ClippingType    clipType = app->menu.getClippingType();
        ClipAlgorithmType clipAlgo = app->menu.getClipAlgorithm();
        COLORREF clipColor = RGB(0, 0, 0);

        if (clipType == ClippingType::Rectangle) {
            static int clickCount = 0;
            static Point topLeft, bottomRight, lineP1, lineP2;
            static std::vector<Point> polygonPoints;

            if (clickCount == 0) {
                topLeft = position; clickCount++;
                app->window.setStatusText(L"Rectangle Clip: click bottom-right corner");
            } else if (clickCount == 1) {
                bottomRight = position;
                LineAlgorithms::drawDDA(hdc, topLeft, Point(bottomRight.x, topLeft.y), clipColor);
                LineAlgorithms::drawDDA(hdc, Point(bottomRight.x, topLeft.y), bottomRight, clipColor);
                LineAlgorithms::drawDDA(hdc, bottomRight, Point(topLeft.x, bottomRight.y), clipColor);
                LineAlgorithms::drawDDA(hdc, Point(topLeft.x, bottomRight.y), topLeft, clipColor);
                clickCount++;
                if (clipAlgo == ClipAlgorithmType::PointClip)   app->window.setStatusText(L"Rectangle drawn. Click the point to clip.");
                else if (clipAlgo == ClipAlgorithmType::LineClip)  app->window.setStatusText(L"Rectangle drawn. Click line start point.");
                else if (clipAlgo == ClipAlgorithmType::PolygonClip) { polygonPoints.clear(); app->window.setStatusText(L"Rectangle drawn. Click polygon vertices. Double-click to finish."); }
            } else if (clickCount >= 2) {
                Clipper c2;
                if (clipAlgo == ClipAlgorithmType::PointClip) {
                    c2.rectanglePointClipping(hdc, topLeft, bottomRight, const_cast<Point&>(position), color);
                    app->persistentDrawings.push_back({PersistentDrawingType::RectangleClipPoint,{topLeft,bottomRight,position},0,0,0,0,0,app->drawingColor});
                    app->window.setStatusText(L"Rectangle-Point clipping done.");
                    clickCount = 0;
                } else if (clipAlgo == ClipAlgorithmType::LineClip) {
                    if (clickCount == 2) { lineP1 = position; clickCount++; app->window.setStatusText(L"Line start set. Click end point."); }
                    else { lineP2 = position; c2.rectangleLineClipping(hdc, topLeft, bottomRight, lineP1, lineP2, color);
                        app->persistentDrawings.push_back({PersistentDrawingType::RectangleClipLine,{topLeft,bottomRight,lineP1,lineP2},0,0,0,0,0,app->drawingColor});
                        app->window.setStatusText(L"Rectangle-Line clipping done."); clickCount = 0; }
                } else if (clipAlgo == ClipAlgorithmType::PolygonClip) {
                    polygonPoints.push_back(position); SetPixel(hdc, position.x, position.y, color);
                    int n = polygonPoints.size();
                    if (n >= 3 && polygonPoints[n-1].x == polygonPoints[n-2].x && polygonPoints[n-1].y == polygonPoints[n-2].y) {
                        polygonPoints.pop_back(); c2.rectanglePolygonClipping(hdc, topLeft, bottomRight, polygonPoints);
                        PersistentDrawing pd; pd.type = PersistentDrawingType::RectangleClipPolygon;
                        pd.points = {topLeft,bottomRight}; pd.polygonPoints = polygonPoints; pd.color = app->drawingColor;
                        pd.radius = pd.innerRadius = pd.outerRadius = pd.quarter = pd.sideLength = 0;
                        app->persistentDrawings.push_back(pd);
                        app->window.setStatusText(L"Rectangle-Polygon clipping done.");
                        polygonPoints.clear(); clickCount = 0;
                    }
                }
            }
        }
        else if (clipType == ClippingType::Square) {
            static int clickCount = 0; static Point topLeft, lineP1, lineP2; static int sideLength = 0;
            if (clickCount == 0) { topLeft = position; clickCount++; app->window.setStatusText(L"Square Clip: click any point to set size"); }
            else if (clickCount == 1) {
                int dx = position.x-topLeft.x, dy = position.y-topLeft.y;
                sideLength = (int)std::round(std::sqrt(dx*dx+dy*dy));
                Point tr(topLeft.x+sideLength,topLeft.y), br(topLeft.x+sideLength,topLeft.y+sideLength), bl(topLeft.x,topLeft.y+sideLength);
                LineAlgorithms::drawDDA(hdc,topLeft,tr,clipColor); LineAlgorithms::drawDDA(hdc,tr,br,clipColor);
                LineAlgorithms::drawDDA(hdc,br,bl,clipColor); LineAlgorithms::drawDDA(hdc,bl,topLeft,clipColor);
                clickCount++;
                if (clipAlgo==ClipAlgorithmType::PointClip) app->window.setStatusText(L"Square drawn. Click the point to clip.");
                else app->window.setStatusText(L"Square drawn. Click line start.");
            } else if (clickCount >= 2) {
                Clipper c2;
                if (clipAlgo==ClipAlgorithmType::PointClip) {
                    c2.squarePointClipping(hdc,topLeft,sideLength,const_cast<Point&>(position),color);
                    app->persistentDrawings.push_back({PersistentDrawingType::SquareClipPoint,{topLeft,position},0,0,0,0,sideLength,app->drawingColor});
                    app->window.setStatusText(L"Square-Point clipping done."); clickCount=0;
                } else if (clipAlgo==ClipAlgorithmType::LineClip) {
                    if (clickCount==2) { lineP1=position; clickCount++; app->window.setStatusText(L"Line start set. Click end."); }
                    else { lineP2=position; c2.squareLineClipping(hdc,topLeft,sideLength,lineP1,lineP2,color);
                        app->persistentDrawings.push_back({PersistentDrawingType::SquareClipLine,{topLeft,lineP1,lineP2},0,0,0,0,sideLength,app->drawingColor});
                        app->window.setStatusText(L"Square-Line clipping done."); clickCount=0; }
                }
            }
        }
        else if (clipType == ClippingType::Circle) {
            static int clickCount = 0; static int radius; static Point center, p2, lineP1, lineP2;
            if (clickCount==0) { center=position; clickCount++; app->window.setStatusText(L"Circle Clip: click a boundary point"); }
            else if (clickCount==1) {
                p2=position; radius=(int)sqrt((double)(center.x-p2.x)*(center.x-p2.x)+(double)(center.y-p2.y)*(center.y-p2.y));
                CircleAlgorithms::drawMidpoint(hdc,center,radius,clipColor); clickCount++;
                app->window.setStatusText(clipAlgo==ClipAlgorithmType::PointClip ? L"Circle drawn. Click the point to clip." : L"Circle drawn. Click line start.");
            } else if (clickCount==2) {
                if (clipAlgo==ClipAlgorithmType::PointClip) {
                    clipper.circlePointClipping(hdc,center,radius,const_cast<Point&>(position),color);
                    app->persistentDrawings.push_back({PersistentDrawingType::CircleClipPoint,{center,position},radius,0,0,0,0,app->drawingColor});
                    app->window.setStatusText(L"Circle-Point clipping done."); clickCount=0;
                } else if (clipAlgo==ClipAlgorithmType::LineClip) { lineP1=position; clickCount++; app->window.setStatusText(L"Line start set. Click end."); }
            } else if (clickCount==3) {
                lineP2=position; clipper.circleLineClipping(hdc,center,radius,lineP1,lineP2,color);
                app->persistentDrawings.push_back({PersistentDrawingType::CircleClipLine,{center,lineP1,lineP2},radius,0,0,0,0,app->drawingColor});
                app->window.setStatusText(L"Circle-Line clipping done."); clickCount=2;
            }
        }
    }

    ReleaseDC(app->window.getCanvasHandle(), hdc);
    app->update();
}

void Application::handleMouseMove(const Point& position, void* context) {
    Application* app = static_cast<Application*>(context);
    app->inputHandler.processMouseMove(position);
    // Show cursor position in status bar
    wchar_t buf[64];
    swprintf_s(buf, L"X: %d   Y: %d", position.x, position.y);
    app->window.setStatusText(buf);
}

void Application::handleRightClick(const Point& position, void* context) {
    Application* app = static_cast<Application*>(context);
    if (app->menu.getMode() == DrawingMode::DrawCurve && app->pendingClicks.size() >= 2) {
        app->addShape(new Curve(app->pendingClicks, 0.5, app->drawingColor));
        app->resetPendingClicks();
        app->window.refresh();
        app->logger.log("Curve drawn.");
        app->window.setStatusText(L"Curve drawn.");
    }
}

// ============================================================
//  handleCommand  — same logic as original, plus status/mode updates
// ============================================================
void Application::handleCommand(int commandId, void* context) {
    Application* app = static_cast<Application*>(context);
    app->resetPendingClicks();

    switch (commandId) {

    // ---- File ----
    case IDM_FILE_CLEAR:
        app->clearShapes();
        app->window.clear();
        app->window.refresh();
        app->window.setStatusText(L"Canvas cleared.");
        app->logger.log("Screen cleared.");
        break;

    case IDM_FILE_SAVE: {
        std::string path;
        std::cout << "[INPUT] Enter file path to save: ";
        std::cin >> path;
        app->fileManager.saveShapes(path, app->shapes);
        std::ofstream file(path, std::ios::app);
        if (file.is_open()) {
            for (const PersistentDrawing& d : app->persistentDrawings) {
                file << "PERSISTENT " << static_cast<int>(d.type) << " "
                     << d.radius << " " << d.innerRadius << " " << d.outerRadius << " "
                     << d.quarter << " " << d.sideLength << " "
                     << (int)d.color.r << " " << (int)d.color.g << " " << (int)d.color.b << " "
                     << (int)d.floodColor.r << " " << (int)d.floodColor.g << " " << (int)d.floodColor.b << " "
                     << (int)d.borderColor.r << " " << (int)d.borderColor.g << " " << (int)d.borderColor.b << " "
                     << d.floodPoint.x << " " << d.floodPoint.y << " " << d.points.size();
                for (const Point& p : d.points) file << " " << p.x << " " << p.y;
                file << " " << d.polygonPoints.size();
                for (const Point& p : d.polygonPoints) file << " " << p.x << " " << p.y;
                file << "\n";
            }
            file.close();
        }
        app->window.setStatusText(L"Project saved.");
        break;
    }

    case IDM_FILE_LOAD: {
        std::string path;
        std::cout << "[INPUT] Enter file path to load: ";
        std::cin >> path;
        app->fileManager.loadShapes(path, app->shapes);
        app->persistentDrawings.clear();
        std::ifstream file(path); std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line); std::string token; ss >> token;
            if (token != "PERSISTENT") continue;
            PersistentDrawing pd;
            int typeInt, r, g, b, fcR, fcG, fcB, bcR, bcG, bcB, fpX, fpY, numPoints, numPolyPoints;
            ss >> typeInt >> pd.radius >> pd.innerRadius >> pd.outerRadius >> pd.quarter >> pd.sideLength
               >> r >> g >> b >> fcR >> fcG >> fcB >> bcR >> bcG >> bcB >> fpX >> fpY >> numPoints;
            pd.type = static_cast<PersistentDrawingType>(typeInt);
            pd.color = Color(r,g,b); pd.floodColor = Color(fcR,fcG,fcB);
            pd.borderColor = Color(bcR,bcG,bcB); pd.floodPoint = Point(fpX,fpY);
            for (int i=0;i<numPoints;i++){int px,py;ss>>px>>py;pd.points.push_back(Point(px,py));}
            ss >> numPolyPoints;
            for (int i=0;i<numPolyPoints;i++){int px,py;ss>>px>>py;pd.polygonPoints.push_back(Point(px,py));}
            app->persistentDrawings.push_back(pd);
        }
        file.close();
        app->window.refresh();
        app->window.setStatusText(L"Project loaded.");
        break;
    }

    // ---- Preferences ----
    case IDM_PREF_BG_WHITE:
        app->setBackgroundColor(Color(255,255,255));
        app->window.clear(); app->window.refresh();
        app->window.setStatusText(L"Background: White");
        break;

    case IDM_PREF_CURSOR_DEFAULT:
        app->preferences.setMouseCursor(MouseCursorType::Default);
        app->window.setCursor(LoadCursor(NULL, IDC_ARROW));
        app->window.setStatusText(L"Cursor: Default");
        break;

    case IDM_PREF_CURSOR_CROSSHAIR:
        app->preferences.setMouseCursor(MouseCursorType::Crosshair);
        app->window.setCursor(LoadCursor(NULL, IDC_CROSS));
        app->window.setStatusText(L"Cursor: Crosshair");
        break;

    case IDM_PREF_CURSOR_HAND:
        app->preferences.setMouseCursor(MouseCursorType::Hand);
        app->window.setCursor(LoadCursor(NULL, IDC_HAND));
        app->window.setStatusText(L"Cursor: Hand");
        break;

    case IDM_PREF_COLOR: {
        static COLORREF customColors[16] = {};
        CHOOSECOLOR cc = {}; cc.lStructSize = sizeof(cc);
        cc.hwndOwner = app->window.getHandle();
        cc.lpCustColors = customColors;
        cc.rgbResult = RGB(app->drawingColor.r, app->drawingColor.g, app->drawingColor.b);
        cc.Flags = CC_FULLOPEN | CC_RGBINIT;
        if (ChooseColor(&cc)) {
            Color chosen(GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));
            app->setDrawingColor(chosen);
            app->window.setStatusText(L"Drawing colour updated.");
        }
        break;
    }

    // ---- Lines ----
    case IDM_DDA_LINE:
        app->menu.setMode(DrawingMode::DrawLine);
        app->menu.setLineAlgorithm(LineAlgorithmType::DDA);
        app->updateStatusMode(L"Drawing Line", L"DDA");
        app->logger.log("Line mode: DDA. Click two points.");
        break;
    case IDM_MIDPOINT_LINE:
        app->menu.setMode(DrawingMode::DrawLine);
        app->menu.setLineAlgorithm(LineAlgorithmType::Midpoint);
        app->updateStatusMode(L"Drawing Line", L"Midpoint");
        break;
    case IDM_PARAMETRIC_LINE:
        app->menu.setMode(DrawingMode::DrawLine);
        app->menu.setLineAlgorithm(LineAlgorithmType::Parametric);
        app->updateStatusMode(L"Drawing Line", L"Parametric");
        break;

    // ---- Circles ----
    case IDM_DIRECT_CIRCLE:
        app->menu.setMode(DrawingMode::DrawCircle);
        app->menu.setCircleAlgorithm(CircleAlgorithmType::Direct);
        app->updateStatusMode(L"Drawing Circle", L"Direct");
        break;
    case IDM_POLAR_CIRCLE:
        app->menu.setMode(DrawingMode::DrawCircle);
        app->menu.setCircleAlgorithm(CircleAlgorithmType::Polar);
        app->updateStatusMode(L"Drawing Circle", L"Polar");
        break;
    case IDM_ITERATIVE_POLAR_CIRCLE:
        app->menu.setMode(DrawingMode::DrawCircle);
        app->menu.setCircleAlgorithm(CircleAlgorithmType::IterativePolar);
        app->updateStatusMode(L"Drawing Circle", L"Iterative Polar");
        break;
    case IDM_MIDPOINT_CIRCLE:
        app->menu.setMode(DrawingMode::DrawCircle);
        app->menu.setCircleAlgorithm(CircleAlgorithmType::Midpoint);
        app->updateStatusMode(L"Drawing Circle", L"Midpoint");
        break;
    case IDM_MODIFIED_MIDPOINT_CIRCLE:
        app->menu.setMode(DrawingMode::DrawCircle);
        app->menu.setCircleAlgorithm(CircleAlgorithmType::ModifiedMidpoint);
        app->updateStatusMode(L"Drawing Circle", L"Modified Midpoint");
        break;

    // ---- Ellipses ----
    case IDM_DIRECT_ELLIPSE:
        app->menu.setMode(DrawingMode::DrawEllipse);
        app->menu.setEllipseAlgorithm(EllipseAlgorithmType::Direct);
        app->updateStatusMode(L"Drawing Ellipse", L"Direct");
        break;
    case IDM_POLAR_ELLIPSE:
        app->menu.setMode(DrawingMode::DrawEllipse);
        app->menu.setEllipseAlgorithm(EllipseAlgorithmType::Polar);
        app->updateStatusMode(L"Drawing Ellipse", L"Polar");
        break;
    case IDM_ITERATIVE_POLAR_ELLIPSE:
        app->menu.setMode(DrawingMode::DrawEllipse);
        app->menu.setEllipseAlgorithm(EllipseAlgorithmType::IterativePolar);
        app->updateStatusMode(L"Drawing Ellipse", L"Iterative Polar");
        break;
    case IDM_MIDPOINT_ELLIPSE:
        app->menu.setMode(DrawingMode::DrawEllipse);
        app->menu.setEllipseAlgorithm(EllipseAlgorithmType::Midpoint);
        app->updateStatusMode(L"Drawing Ellipse", L"Midpoint");
        break;

    // ---- Curves ----
    case IDM_CARDINAL_SPLINE_CURVE:
        app->menu.setMode(DrawingMode::DrawCurve);
        app->updateStatusMode(L"Drawing Curve", L"Cardinal Spline");
        break;

    // ---- Filling ----
    case IDM_FILL_CIRCLE_WITH_LINES:
        app->menu.setMode(DrawingMode::Fill);
        app->menu.setFillAlgorithm(FillAlgorithmType::CircleFillWithLines);
        std::cout << "[INPUT] Quarter (1-4): "; std::cin >> app->fillQuarter;
        if (app->fillQuarter < 1 || app->fillQuarter > 4) app->fillQuarter = 1;
        app->updateStatusMode(L"Fill", L"Circle w/ Lines");
        break;
    case IDM_FILL_CIRCLE_WITH_CIRCLES:
        app->menu.setMode(DrawingMode::Fill);
        app->menu.setFillAlgorithm(FillAlgorithmType::CircleFillWithCircles);
        std::cout << "[INPUT] Quarter (1-4): "; std::cin >> app->fillQuarter;
        if (app->fillQuarter < 1 || app->fillQuarter > 4) app->fillQuarter = 1;
        app->updateStatusMode(L"Fill", L"Circle w/ Circles");
        break;
    case IDM_FILL_RECTANGLE_CURVES:
        app->menu.setMode(DrawingMode::Fill);
        app->menu.setFillAlgorithm(FillAlgorithmType::RectangleFillWithCurves);
        app->updateStatusMode(L"Fill", L"Rectangle w/ Bezier");
        break;
    case IDM_FILL_SQUARE_CURVES:
        app->menu.setMode(DrawingMode::Fill);
        app->menu.setFillAlgorithm(FillAlgorithmType::SquareFillWithCurves);
        app->updateStatusMode(L"Fill", L"Square w/ Hermite");
        break;
    case IDM_FILL_CONVEX:
        app->menu.setMode(DrawingMode::Fill);
        app->menu.setFillAlgorithm(FillAlgorithmType::ConvexFill);
        app->updateStatusMode(L"Fill", L"Convex Polygon");
        break;
    case IDM_FILL_NON_CONVEX:
        app->menu.setMode(DrawingMode::Fill);
        app->menu.setFillAlgorithm(FillAlgorithmType::NonConvexFill);
        app->updateStatusMode(L"Fill", L"Non-Convex Polygon");
        break;
    case IDM_FILL_FLOOD_RECURSIVE: {
        app->menu.setMode(DrawingMode::Fill);
        app->menu.setFillAlgorithm(FillAlgorithmType::FloodFillRecursive);
        int r,g,b; std::cout << "[INPUT] Fill colour R G B: "; std::cin >> r >> g >> b;
        app->floodFillingColor = Color(r,g,b);
        app->updateStatusMode(L"Fill", L"Flood (Recursive)");
        break;
    }
    case IDM_FILL_FLOOD_NON_RECURSIVE: {
        app->menu.setMode(DrawingMode::Fill);
        app->menu.setFillAlgorithm(FillAlgorithmType::FloodFillNonRecursive);
        int r,g,b; std::cout << "[INPUT] Fill colour R G B: "; std::cin >> r >> g >> b;
        app->floodFillingColor = Color(r,g,b);
        app->updateStatusMode(L"Fill", L"Flood (Non-Recursive)");
        break;
    }

    // ---- Clipping ----
    case IDM_CLIPPING_RECTANGLE_POINT:
        app->menu.setMode(DrawingMode::Clip);
        app->menu.setClippingType(ClippingType::Rectangle);
        app->menu.setClipAlgorithm(ClipAlgorithmType::PointClip);
        app->updateStatusMode(L"Clip", L"Rectangle · Point");
        break;
    case IDM_CLIPPING_RECTANGLE_LINE:
        app->menu.setMode(DrawingMode::Clip);
        app->menu.setClippingType(ClippingType::Rectangle);
        app->menu.setClipAlgorithm(ClipAlgorithmType::LineClip);
        app->updateStatusMode(L"Clip", L"Rectangle · Line");
        break;
    case IDM_CLIPPING_RECTANGLE_POLYGON:
        app->menu.setMode(DrawingMode::Clip);
        app->menu.setClippingType(ClippingType::Rectangle);
        app->menu.setClipAlgorithm(ClipAlgorithmType::PolygonClip);
        app->updateStatusMode(L"Clip", L"Rectangle · Polygon");
        break;
    case IDM_CLIPPING_SQUARE_POINT:
        app->menu.setMode(DrawingMode::Clip);
        app->menu.setClippingType(ClippingType::Square);
        app->menu.setClipAlgorithm(ClipAlgorithmType::PointClip);
        app->updateStatusMode(L"Clip", L"Square · Point");
        break;
    case IDM_CLIPPING_SQUARE_LINE:
        app->menu.setMode(DrawingMode::Clip);
        app->menu.setClippingType(ClippingType::Square);
        app->menu.setClipAlgorithm(ClipAlgorithmType::LineClip);
        app->updateStatusMode(L"Clip", L"Square · Line");
        break;
    case IDM_CLIPPING_CIRCLE_POINT:
        app->menu.setMode(DrawingMode::Clip);
        app->menu.setClippingType(ClippingType::Circle);
        app->menu.setClipAlgorithm(ClipAlgorithmType::PointClip);
        app->updateStatusMode(L"Clip", L"Circle · Point");
        break;
    case IDM_CLIPPING_CIRCLE_LINE:
        app->menu.setMode(DrawingMode::Clip);
        app->menu.setClippingType(ClippingType::Circle);
        app->menu.setClipAlgorithm(ClipAlgorithmType::LineClip);
        app->updateStatusMode(L"Clip", L"Circle · Line");
        break;

    // ---- Faces ----
    case IDM_SMILEY_FACE_HAPPY:
        app->menu.setMode(DrawingMode::DrawSmiley);
        app->pendingHappyFace = true;
        app->updateStatusMode(L"Drawing Face", L"Happy");
        break;
    case IDM_SMILEY_FACE_SAD:
        app->menu.setMode(DrawingMode::DrawSmiley);
        app->pendingHappyFace = false;
        app->updateStatusMode(L"Drawing Face", L"Sad");
        break;

    default: break;
    }
}

// ============================================================
//  replayPersistentDrawing  (unchanged from original)
// ============================================================
void Application::replayPersistentDrawing(HDC hdc, const PersistentDrawing& drawing) {
    COLORREF color = RGB(drawing.color.r, drawing.color.g, drawing.color.b);
    switch (drawing.type) {
        case PersistentDrawingType::CircleFillWithLines:
            CircleAlgorithms::drawMidpoint(hdc, drawing.points[0], drawing.radius, color);
            FillAlgorithms::fillCircleWithLines(hdc, drawing.points[0], drawing.radius, drawing.quarter, drawing.color);
            break;
        case PersistentDrawingType::CircleFillWithCircles:
            CircleAlgorithms::drawMidpoint(hdc, drawing.points[0], drawing.outerRadius, color);
            FillAlgorithms::fillCircleWithCircles(hdc, drawing.points[0], drawing.innerRadius, drawing.outerRadius, drawing.quarter, drawing.color, drawing.color);
            break;
        case PersistentDrawingType::RectangleFillWithCurves:
            FillAlgorithms::fillRectangleWithCurves(hdc, drawing.points[0], drawing.points[1], color);
            break;
        case PersistentDrawingType::SquareFillWithCurves:
            FillAlgorithms::fillSquareWithCurves(hdc, drawing.points[0], drawing.sideLength, color);
            break;
        case PersistentDrawingType::RectangleClipPoint: {
            Point tl=drawing.points[0], br=drawing.points[1], pt=drawing.points[2];
            COLORREF black=RGB(0,0,0);
            LineAlgorithms::drawDDA(hdc,tl,Point(br.x,tl.y),black); LineAlgorithms::drawDDA(hdc,Point(br.x,tl.y),br,black);
            LineAlgorithms::drawDDA(hdc,br,Point(tl.x,br.y),black); LineAlgorithms::drawDDA(hdc,Point(tl.x,br.y),tl,black);
            Clipper c; c.rectanglePointClipping(hdc,tl,br,pt,color); break; }
        case PersistentDrawingType::RectangleClipLine: {
            Point tl=drawing.points[0],br=drawing.points[1],lp1=drawing.points[2],lp2=drawing.points[3];
            COLORREF black=RGB(0,0,0);
            LineAlgorithms::drawDDA(hdc,tl,Point(br.x,tl.y),black); LineAlgorithms::drawDDA(hdc,Point(br.x,tl.y),br,black);
            LineAlgorithms::drawDDA(hdc,br,Point(tl.x,br.y),black); LineAlgorithms::drawDDA(hdc,Point(tl.x,br.y),tl,black);
            Clipper c; c.rectangleLineClipping(hdc,tl,br,lp1,lp2,color); break; }
        case PersistentDrawingType::RectangleClipPolygon: {
            Point tl=drawing.points[0],br=drawing.points[1];
            COLORREF black=RGB(0,0,0);
            LineAlgorithms::drawDDA(hdc,tl,Point(br.x,tl.y),black); LineAlgorithms::drawDDA(hdc,Point(br.x,tl.y),br,black);
            LineAlgorithms::drawDDA(hdc,br,Point(tl.x,br.y),black); LineAlgorithms::drawDDA(hdc,Point(tl.x,br.y),tl,black);
            Clipper c; c.rectanglePolygonClipping(hdc,tl,br,const_cast<std::vector<Point>&>(drawing.polygonPoints)); break; }
        case PersistentDrawingType::SquareClipPoint: {
            Point tl=drawing.points[0],pt=drawing.points[1]; int side=drawing.sideLength;
            COLORREF black=RGB(0,0,0);
            Point tr(tl.x+side,tl.y),br(tl.x+side,tl.y+side),bl(tl.x,tl.y+side);
            LineAlgorithms::drawDDA(hdc,tl,tr,black); LineAlgorithms::drawDDA(hdc,tr,br,black);
            LineAlgorithms::drawDDA(hdc,br,bl,black); LineAlgorithms::drawDDA(hdc,bl,tl,black);
            Clipper c; c.squarePointClipping(hdc,tl,side,pt,color); break; }
        case PersistentDrawingType::SquareClipLine: {
            Point tl=drawing.points[0],lp1=drawing.points[1],lp2=drawing.points[2]; int side=drawing.sideLength;
            COLORREF black=RGB(0,0,0);
            Point tr(tl.x+side,tl.y),br(tl.x+side,tl.y+side),bl(tl.x,tl.y+side);
            LineAlgorithms::drawDDA(hdc,tl,tr,black); LineAlgorithms::drawDDA(hdc,tr,br,black);
            LineAlgorithms::drawDDA(hdc,br,bl,black); LineAlgorithms::drawDDA(hdc,bl,tl,black);
            Clipper c; c.squareLineClipping(hdc,tl,side,lp1,lp2,color); break; }
        case PersistentDrawingType::CircleClipPoint: {
            Clipper c; CircleAlgorithms::drawMidpoint(hdc,drawing.points[0],drawing.radius,RGB(0,0,0));
            c.circlePointClipping(hdc,drawing.points[0],drawing.radius,drawing.points[1],color); break; }
        case PersistentDrawingType::CircleClipLine: {
            Clipper c; CircleAlgorithms::drawMidpoint(hdc,drawing.points[0],drawing.radius,RGB(0,0,0));
            c.circleLineClipping(hdc,drawing.points[0],drawing.radius,drawing.points[1],drawing.points[2],color); break; }
        case PersistentDrawingType::FloodFillRecursive:
            FillAlgorithms::floodFillRecursive(hdc, drawing.floodPoint, drawing.floodColor, drawing.borderColor); break;
        case PersistentDrawingType::FloodFillNonRecursive:
            FillAlgorithms::floodFillNonRecursive(hdc, drawing.floodPoint, drawing.floodColor, drawing.borderColor); break;
        case PersistentDrawingType::ConvexPolygonFill: {
            for (int i=0;i<(int)drawing.polygonPoints.size();i++)
                LineAlgorithms::drawDDA(hdc,drawing.polygonPoints[i],drawing.polygonPoints[(i+1)%drawing.polygonPoints.size()],color);
            FillAlgorithms::convexFill(hdc,drawing.polygonPoints,(int)drawing.polygonPoints.size(),color); break; }
        case PersistentDrawingType::NonConvexPolygonFill: {
            for (int i=0;i<(int)drawing.polygonPoints.size();i++)
                LineAlgorithms::drawDDA(hdc,drawing.polygonPoints[i],drawing.polygonPoints[(i+1)%drawing.polygonPoints.size()],color);
            FillAlgorithms::nonConvexFill(hdc,drawing.polygonPoints,(int)drawing.polygonPoints.size(),color); break; }
    }
}

// ============================================================
//  drawPendingClickMarkers  (unchanged)
// ============================================================
void Application::drawPendingClickMarkers(HDC hdc) {
    if (pendingClicks.empty()) return;
    HPEN p = CreatePen(PS_SOLID, 1, RGB(255,0,0));
    HBRUSH b = CreateSolidBrush(RGB(255,0,0));
    HPEN op = (HPEN)SelectObject(hdc, p); HBRUSH ob = (HBRUSH)SelectObject(hdc, b);
    for (const Point& pt : pendingClicks) Ellipse(hdc, pt.x-4, pt.y-4, pt.x+4, pt.y+4);
    SelectObject(hdc, ob); SelectObject(hdc, op);
    DeleteObject(b); DeleteObject(p);
}

// ============================================================
//  addSmileyFace  (unchanged)
// ============================================================
void Application::addSmileyFace(const Point& center, bool happy) {
    const int faceRadius = 120;
    addShape(new Circle(center, faceRadius, CircleAlgorithmType::Midpoint, drawingColor));
    addShape(new Circle(Point(center.x-45, center.y-35), 16, CircleAlgorithmType::Midpoint, drawingColor));
    addShape(new Circle(Point(center.x+45, center.y-35), 16, CircleAlgorithmType::Midpoint, drawingColor));
    addShape(new Line(Point(center.x, center.y-10), Point(center.x-15, center.y+35), LineAlgorithmType::Midpoint, drawingColor));
    addShape(new Line(Point(center.x-15, center.y+35), Point(center.x+15, center.y+35), LineAlgorithmType::Midpoint, drawingColor));
    std::vector<Point> mouth;
    if (happy) {
        mouth.push_back(Point(center.x-55, center.y+45)); mouth.push_back(Point(center.x-25, center.y+75));
        mouth.push_back(Point(center.x+25, center.y+75)); mouth.push_back(Point(center.x+55, center.y+45));
    } else {
        mouth.push_back(Point(center.x-55, center.y+80)); mouth.push_back(Point(center.x-25, center.y+50));
        mouth.push_back(Point(center.x+25, center.y+50)); mouth.push_back(Point(center.x+55, center.y+80));
    }
    addShape(new Curve(mouth, 0.5, drawingColor));
    window.refresh();
    logger.log(happy ? "Happy face drawn." : "Sad face drawn.");
    window.setStatusText(happy ? L"Happy face drawn." : L"Sad face drawn.");
}