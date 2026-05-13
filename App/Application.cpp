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
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

Application::Application()
    : window(800, 600, Color(255, 255, 255)),
      inputHandler(),
      menu(),
      fileManager(),
      preferences(),
      logger(),
      shapes(),
      pendingClicks(),
      fillQuarter(1),
      drawingColor(0, 0, 0),
      backgroundColor(255, 255, 255) {
    window.setPaintCallback(Application::handlePaint, this);
    window.setMouseClickCallback(Application::handleMouseClick, this);
    window.setMouseMoveCallback(Application::handleMouseMove, this);
    window.setCommandCallback(Application::handleCommand, this);
}

Application::~Application() {
    // Release owned shapes.
    for (Shape* shape : shapes) {
        delete shape;
    }
    shapes.clear();
}

void Application::run() {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$",  "r", stdin);

    logger.log("Application started.");
    logger.log("Use the menus at the top of the window to draw shapes.");

    if (!window.create(L"Drawing Package")) {
        logger.error("Failed to create window.");
        return;
    }

    window.runMessageLoop();
}

void Application::update() {
    // TODO: Process input and menu state.
}

void Application::render(HDC hdc) {
    // Draw all stored shapes using the provided WinAPI HDC.
    for (Shape* shape : shapes) {
        shape->draw(hdc);
    }
}

void Application::addShape(Shape* shape) {
    if (shape) shapes.push_back(shape);
}

void Application::clearShapes() {
    // Release owned shapes.
    for (Shape* shape : shapes) {
        delete shape;
    }
    shapes.clear();
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

void Application::resetPendingClicks() {
    pendingClicks.clear();
}

void Application::addSmileyFace(bool happy) {
    const Color faceColor = drawingColor;
    const Point center(400, 280);
    const int faceRadius = 120;

    addShape(new Circle(center, faceRadius, CircleAlgorithmType::Midpoint, faceColor));
    addShape(new Circle(Point(center.x - 45, center.y - 35), 16, CircleAlgorithmType::Midpoint, faceColor));
    addShape(new Circle(Point(center.x + 45, center.y - 35), 16, CircleAlgorithmType::Midpoint, faceColor));

    addShape(new Line(
        Point(center.x, center.y - 10),
        Point(center.x - 15, center.y + 35),
        LineAlgorithmType::Midpoint,
        faceColor));
    addShape(new Line(
        Point(center.x - 15, center.y + 35),
        Point(center.x + 15, center.y + 35),
        LineAlgorithmType::Midpoint,
        faceColor));

    std::vector<Point> mouthPoints;
    if (happy) {
        mouthPoints.push_back(Point(center.x - 55, center.y + 45));
        mouthPoints.push_back(Point(center.x - 25, center.y + 75));
        mouthPoints.push_back(Point(center.x + 25, center.y + 75));
        mouthPoints.push_back(Point(center.x + 55, center.y + 45));
    } else {
        mouthPoints.push_back(Point(center.x - 55, center.y + 80));
        mouthPoints.push_back(Point(center.x - 25, center.y + 50));
        mouthPoints.push_back(Point(center.x + 25, center.y + 50));
        mouthPoints.push_back(Point(center.x + 55, center.y + 80));
    }
    addShape(new Curve(mouthPoints, 0.5, faceColor));

    window.refresh();
    logger.log(happy ? "Happy face drawn." : "Sad face drawn.");
}

void Application::handlePaint(HDC hdc, void* context) {
    Application* app = static_cast<Application*>(context);
    app->render(hdc);
}

void Application::handleMouseClick(const Point& position, void* context) {
    Application* app = static_cast<Application*>(context);
    app->inputHandler.processMouseClick(position);

    HDC hdc = GetDC(app->window.getHandle());
    DrawingMode mode = app->menu.getMode();
    COLORREF color = RGB(app->drawingColor.r, app->drawingColor.g, app->drawingColor.b);

    if (mode == DrawingMode::DrawLine) {
        app->pendingClicks.push_back(position);
        if (app->pendingClicks.size() == 1) {
            app->logger.log("Line start set. Click line end.");
        } else if (app->pendingClicks.size() == 2) {
            app->addShape(new Line(
                app->pendingClicks[0],
                app->pendingClicks[1],
                app->menu.getLineAlgorithm(),
                app->drawingColor));
            app->resetPendingClicks();
            app->window.refresh();
            app->logger.log("Line drawn.");
        }
    }
    else if (mode == DrawingMode::DrawCircle) {
        app->pendingClicks.push_back(position);
        if (app->pendingClicks.size() == 1) {
            app->logger.log("Circle center set. Click a boundary point.");
        } else if (app->pendingClicks.size() == 2) {
            const Point& center = app->pendingClicks[0];
            const Point& boundary = app->pendingClicks[1];
            int dx = boundary.x - center.x;
            int dy = boundary.y - center.y;
            int radius = static_cast<int>(std::round(std::sqrt(dx * dx + dy * dy)));

            app->addShape(new Circle(
                center,
                radius,
                app->menu.getCircleAlgorithm(),
                app->drawingColor));
            app->resetPendingClicks();
            app->window.refresh();
            app->logger.log("Circle drawn.");
        }
    }
    else if (mode == DrawingMode::DrawEllipse) {
        app->pendingClicks.push_back(position);
        if (app->pendingClicks.size() == 1) {
            app->logger.log("Ellipse center set. Click a point for horizontal radius.");
        } else if (app->pendingClicks.size() == 2) {
            app->logger.log("Ellipse horizontal radius set. Click a point for vertical radius.");
        } else if (app->pendingClicks.size() == 3) {
            const Point& center = app->pendingClicks[0];
            const Point& radiusXPoint = app->pendingClicks[1];
            const Point& radiusYPoint = app->pendingClicks[2];
            int radiusX = std::abs(radiusXPoint.x - center.x);
            int radiusY = std::abs(radiusYPoint.y - center.y);

            app->addShape(new MyEllipse(
                center,
                radiusX,
                radiusY,
                app->menu.getEllipseAlgorithm(),
                app->drawingColor));
            app->resetPendingClicks();
            app->window.refresh();
            app->logger.log("Ellipse drawn.");
        }
    }
    else if (mode == DrawingMode::DrawCurve) {
        app->pendingClicks.push_back(position);
        if (app->pendingClicks.size() < 4) {
            app->logger.log("Curve control point set. Click until 4 points are selected.");
        } else {
            app->addShape(new Curve(app->pendingClicks, 0.5, app->drawingColor));
            app->resetPendingClicks();
            app->window.refresh();
            app->logger.log("Cardinal spline curve drawn.");
        }
    }
    else if (mode == DrawingMode::Fill) {
        FillAlgorithmType fillType = app->menu.getFillAlgorithm();
        if (fillType == FillAlgorithmType::CircleFillWithLines) {
            app->pendingClicks.push_back(position);
            if (app->pendingClicks.size() == 1) {
                app->logger.log("Fill Circle with lines: center set. Click a boundary point.");
            } else if (app->pendingClicks.size() == 2) {
                const Point& center = app->pendingClicks[0];
                const Point& boundary = app->pendingClicks[1];
                int dx = boundary.x - center.x;
                int dy = boundary.y - center.y;
                int radius = static_cast<int>(std::round(std::sqrt(dx * dx + dy * dy)));

                CircleAlgorithms::drawMidpoint(hdc, center, radius, color);
                FillAlgorithms::fillCircleWithLines(hdc, center, radius, app->fillQuarter, app->drawingColor);
                app->resetPendingClicks();
                app->logger.log("Fill Circle with lines done.");
            }
        }
        else if (fillType == FillAlgorithmType::CircleFillWithCircles) {
            app->pendingClicks.push_back(position);
            if (app->pendingClicks.size() == 1) {
                app->logger.log("Fill Circle with circles: center set. Click inner radius point.");
            } else if (app->pendingClicks.size() == 2) {
                app->logger.log("Inner radius set. Click outer radius point.");
            } else if (app->pendingClicks.size() == 3) {
                const Point& center = app->pendingClicks[0];
                const Point& innerPoint = app->pendingClicks[1];
                const Point& outerPoint = app->pendingClicks[2];
                int innerDx = innerPoint.x - center.x;
                int innerDy = innerPoint.y - center.y;
                int outerDx = outerPoint.x - center.x;
                int outerDy = outerPoint.y - center.y;
                int innerRadius = static_cast<int>(std::round(std::sqrt(innerDx * innerDx + innerDy * innerDy)));
                int outerRadius = static_cast<int>(std::round(std::sqrt(outerDx * outerDx + outerDy * outerDy)));

                CircleAlgorithms::drawMidpoint(hdc, center, outerRadius, color);
                FillAlgorithms::fillCircleWithCircles(
                    hdc,
                    center,
                    innerRadius,
                    outerRadius,
                    app->fillQuarter,
                    app->drawingColor,
                    app->drawingColor);
                app->resetPendingClicks();
                app->logger.log("Fill Circle with circles done.");
            }
        }
        else if (fillType == FillAlgorithmType::SquareFillWithCurves) {
            static int clickCount = 0;
            static Point topLeft;

            if (clickCount == 0) {
                topLeft = position;
                clickCount = 1;
                app->logger.log("Fill Square: top-left set. Click any point to set size.");
            }
            else {
                int side = sqrt((topLeft.x-position.x) * (topLeft.x-position.x) + (topLeft.y-position.y) * (topLeft.y-position.y));
                FillAlgorithms::fillSquareWithCurves(hdc, topLeft, side, color);
                app->logger.log("Fill Square done.");
                clickCount = 0;
            }
        }
        else if (fillType == FillAlgorithmType::RectangleFillWithCurves) {
            static int clickCount = 0;
            static Point topLeft;

            if (clickCount == 0) {
                topLeft = position;
                clickCount = 1;
                app->logger.log("Fill Rectangle: top-left set. Click bottom-right.");
            }
            else {
                FillAlgorithms::fillRectangleWithCurves(hdc, topLeft, position, color);
                app->logger.log("Fill Rectangle done.");
                clickCount = 0;
            }
        }
    }
    else if (mode == DrawingMode::Clip) {
        static Clipper clipper;
        static int clickCount = 0;

        ClippingType    clipType = app->menu.getClippingType();
        ClipAlgorithmType clipAlgo = app->menu.getClipAlgorithm();
        COLORREF clipColor = RGB(255, 255, 255);

        if (clipType == ClippingType::Rectangle) {

        }
        else if (clipType == ClippingType::Square) {

        }
        else if (clipType == ClippingType::Circle) {
            int radius;
            Point center, p2, lineP1, lineP2;
            if (clickCount == 0) {
                center = position;
                clickCount++;
                app->logger.log("Circle clip: center set. Click a point on the circle boundary.");
            }
            else if (clickCount == 1) {
                p2 = position;
                radius = sqrt((center.x-p2.x) * (center.x-p2.x) + (center.y-p2.y) * (center.y-p2.y));
                CircleAlgorithms::drawMidpoint(hdc, center, radius, clipColor);
                clickCount++;
                if (clipAlgo == ClipAlgorithmType::PointClip)
                    app->logger.log("Circle window drawn. Click the point to clip.");
                else
                    app->logger.log("Circle window drawn. Click start of the line to clip.");
            }
            else if (clickCount == 3) {
                if (clipAlgo == ClipAlgorithmType::PointClip) {
                    clipper.circlePointClipping(hdc, center, radius, const_cast<Point&>(position), color);
                    app->logger.log("Circle-Point clipping done.");
                    clickCount = 3; // allow user to enter point again
                }
                else if (clipAlgo == ClipAlgorithmType::LineClip) {
                    lineP1 = position;
                    clickCount++;
                    app->logger.log("Line start set. Click line end.");
                }
                else if (clickCount == 4) {
                    lineP2 = position;
                    clipper.circleLineClipping(hdc, center, radius, lineP1, lineP2, color);
                    clickCount = 3; // allow user to enter line again
                }
            }

        }

    }
    ReleaseDC(app->window.getHandle(), hdc);
    app->update();
}

void Application::handleMouseMove(const Point& position, void* context) {
    Application* app = static_cast<Application*>(context);
    app->inputHandler.processMouseMove(position);
}

// handleCommand
// Dispatches every menu click (WM_COMMAND) to the right action.

void Application::handleCommand(int commandId, void* context) {
    Application* app = static_cast<Application*>(context);
    app->resetPendingClicks();

    switch (commandId) {

        //File Menu commands
        case IDM_FILE_CLEAR:
            // 1a: clear all shapes from the screen
            app->clearShapes();
            app->window.clear();
            app->window.refresh();
            app->logger.log("Screen cleared.");
            break;

        case IDM_FILE_SAVE: {
            //1b: save all drawn shapes to a file
            // Requirement 10: take file path from the console
            std::string path;
            std::cout << "[INPUT] Enter file path to save (e.g. drawing.cg): ";
            std::cin  >> path;
            app->fileManager.saveShapes(path, app->shapes);
            break;
        }

        case IDM_FILE_LOAD: {
            //1c: load saved shapes from a file
            std::string path;
            std::cout << "[INPUT] Enter file path to load: ";
            std::cin  >> path;
            app->fileManager.loadShapes(path, app->shapes);
            app->window.refresh();   // Repaint so loaded shapes appear
            break;
        }

        // Preferences Menu commands
        case IDM_PREF_BG_WHITE:
            // Assignment 2a: change window background to white
            app->setBackgroundColor(Color(255, 255, 255));
            app->window.clear();
            app->window.refresh();
            app->logger.log("Background set to white.");
            break;

        case IDM_PREF_CURSOR_DEFAULT:
            // 2b: change mouse cursor shape
            app->preferences.setMouseCursor(MouseCursorType::Default);
            app->window.setCursor(LoadCursor(NULL, IDC_ARROW));
            app->logger.log("Cursor: Default.");
            break;

        case IDM_PREF_CURSOR_CROSSHAIR:
            app->preferences.setMouseCursor(MouseCursorType::Crosshair);
            app->window.setCursor(LoadCursor(NULL, IDC_CROSS));
            app->logger.log("Cursor: Crosshair.");
            break;

        case IDM_PREF_CURSOR_HAND:
            app->preferences.setMouseCursor(MouseCursorType::Hand);
            app->window.setCursor(LoadCursor(NULL, IDC_HAND));
            app->logger.log("Cursor: Hand.");
            break;

        case IDM_PREF_COLOR: {
            // 2c: let the user choose the drawing color from a dialog
            static COLORREF customColors[16] = {};
            CHOOSECOLOR cc    = {};
            cc.lStructSize    = sizeof(cc);
            cc.hwndOwner      = app->window.getHandle();
            cc.lpCustColors   = customColors;
            cc.rgbResult      = RGB(app->drawingColor.r, app->drawingColor.g, app->drawingColor.b);
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;

            if (ChooseColor(&cc)) {
                Color chosen(GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));
                app->setDrawingColor(chosen);
                app->logger.log("Drawing color updated via color picker.");
            }
            break;
        }

        // Line Menu
        case IDM_DDA_LINE:
            app->menu.setMode(DrawingMode::DrawLine);
            app->menu.setLineAlgorithm(LineAlgorithmType::DDA);
            app->logger.log("Line mode: DDA. Click two points to draw a line.");
            break;

        case IDM_MIDPOINT_LINE:
            app->menu.setMode(DrawingMode::DrawLine);
            app->menu.setLineAlgorithm(LineAlgorithmType::Midpoint);
            app->logger.log("Line mode: Midpoint. Click two points to draw a line.");
            break;

        case IDM_PARAMETRIC_LINE:
            app->menu.setMode(DrawingMode::DrawLine);
            app->menu.setLineAlgorithm(LineAlgorithmType::Parametric);
            app->logger.log("Line mode: Parametric. Click two points to draw a line.");
            break;

        // Circle Menu
        case IDM_DIRECT_CIRCLE:
            app->menu.setMode(DrawingMode::DrawCircle);
            app->menu.setCircleAlgorithm(CircleAlgorithmType::Direct);
            app->logger.log("Circle mode: Direct. Click center, then a boundary point.");
            break;

        case IDM_POLAR_CIRCLE:
            app->menu.setMode(DrawingMode::DrawCircle);
            app->menu.setCircleAlgorithm(CircleAlgorithmType::Polar);
            app->logger.log("Circle mode: Polar. Click center, then a boundary point.");
            break;

        case IDM_ITERATIVE_POLAR_CIRCLE:
            app->menu.setMode(DrawingMode::DrawCircle);
            app->menu.setCircleAlgorithm(CircleAlgorithmType::IterativePolar);
            app->logger.log("Circle mode: Iterative Polar. Click center, then a boundary point.");
            break;

        case IDM_MIDPOINT_CIRCLE:
            app->menu.setMode(DrawingMode::DrawCircle);
            app->menu.setCircleAlgorithm(CircleAlgorithmType::Midpoint);
            app->logger.log("Circle mode: Midpoint. Click center, then a boundary point.");
            break;

        case IDM_MODIFIED_MIDPOINT_CIRCLE:
            app->menu.setMode(DrawingMode::DrawCircle);
            app->menu.setCircleAlgorithm(CircleAlgorithmType::ModifiedMidpoint);
            app->logger.log("Circle mode: Modified Midpoint. Click center, then a boundary point.");
            break;

        // Ellipse Menu
        case IDM_DIRECT_ELLIPSE:
            app->menu.setMode(DrawingMode::DrawEllipse);
            app->menu.setEllipseAlgorithm(EllipseAlgorithmType::Direct);
            app->logger.log("Ellipse mode: Direct. Click center, then radius points.");
            break;

        case IDM_POLAR_ELLIPSE:
            app->menu.setMode(DrawingMode::DrawEllipse);
            app->menu.setEllipseAlgorithm(EllipseAlgorithmType::Polar);
            app->logger.log("Ellipse mode: Polar. Click center, then radius points.");
            break;

        case IDM_ITERATIVE_POLAR_ELLIPSE:
            app->menu.setMode(DrawingMode::DrawEllipse);
            app->menu.setEllipseAlgorithm(EllipseAlgorithmType::IterativePolar);
            app->logger.log("Ellipse mode: Iterative Polar. Click center, then radius points.");
            break;

        case IDM_MIDPOINT_ELLIPSE:
            app->menu.setMode(DrawingMode::DrawEllipse);
            app->menu.setEllipseAlgorithm(EllipseAlgorithmType::Midpoint);
            app->logger.log("Ellipse mode: Midpoint. Click center, then radius points.");
            break;

        // Curves Menu
        case IDM_CARDINAL_SPLINE_CURVE:
            app->menu.setMode(DrawingMode::DrawCurve);
            app->logger.log("Curve mode: Cardinal Spline. Click control points to draw a curve.");
            break;

        // Filling Menu
        case IDM_FILL_CIRCLE_WITH_LINES:
            app->menu.setMode(DrawingMode::Fill);
            app->menu.setFillAlgorithm(FillAlgorithmType::CircleFillWithLines);
            std::cout << "[INPUT] Choose filling quarter (1 upper-right, 2 upper-left, 3 lower-left, 4 lower-right): ";
            std::cin >> app->fillQuarter;
            if (app->fillQuarter < 1 || app->fillQuarter > 4) {
                app->fillQuarter = 1;
                app->logger.log("Invalid quarter. Using quarter 1.");
            }
            app->logger.log("Fill Circle with lines: click center, then edge.");
            break;

        case IDM_FILL_CIRCLE_WITH_CIRCLES:
            app->menu.setMode(DrawingMode::Fill);
            app->menu.setFillAlgorithm(FillAlgorithmType::CircleFillWithCircles);
            std::cout << "[INPUT] Choose filling quarter (1 upper-right, 2 upper-left, 3 lower-left, 4 lower-right): ";
            std::cin >> app->fillQuarter;
            if (app->fillQuarter < 1 || app->fillQuarter > 4) {
                app->fillQuarter = 1;
                app->logger.log("Invalid quarter. Using quarter 1.");
            }
            app->logger.log("Fill Circle with circles: click center, inner radius, outer radius.");
            break;

        case IDM_FILL_RECTANGLE_CURVES:
            app->menu.setMode(DrawingMode::Fill);
            app->menu.setFillAlgorithm(FillAlgorithmType::RectangleFillWithCurves);
            app->logger.log("Fill Rectangle with Bezier Curves [Horizontal]: click top-left, then bottom-right.");
            break;

        case IDM_FILL_SQUARE_CURVES:
            app->menu.setMode(DrawingMode::Fill);
            app->menu.setFillAlgorithm(FillAlgorithmType::SquareFillWithCurves);
            app->logger.log("Fill Square with Hermite Curves [Vertical]: click top-left, then any point for side length.");
            break;

        case IDM_FILL_CONVEX:
            app->menu.setMode(DrawingMode::Fill);
            app->menu.setFillAlgorithm(FillAlgorithmType::ConvexFill);
            app->logger.log("Convex Polygon Fill: click inside any Convex Polygon.");
            break;

        case IDM_FILL_NON_CONVEX:
            app->menu.setMode(DrawingMode::Fill);
            app->menu.setFillAlgorithm(FillAlgorithmType::NonConvexFill);
            app->logger.log("Non-Convex Polygon Fill: click inside any Non-Convex Polygon.");
            break;

        case IDM_FILL_FLOOD_RECURSIVE:
            app->menu.setMode(DrawingMode::Fill);
            app->menu.setFillAlgorithm(FillAlgorithmType::FloodFillRecursive);
            app->logger.log("Flood Fill (Recursive): click inside any closed shape.");
            break;

        case IDM_FILL_FLOOD_NON_RECURSIVE:
            app->menu.setMode(DrawingMode::Fill);
            app->menu.setFillAlgorithm(FillAlgorithmType::FloodFillNonRecursive);
            app->logger.log("Flood Fill (Non-Recursive): click inside any closed shape.");
            break;

        // Clipping Menu
        case IDM_CLIPPING_RECTANGLE_POINT:
            app->menu.setMode(DrawingMode::Clip);
            app->menu.setClippingType(ClippingType::Rectangle);
            app->menu.setClipAlgorithm(ClipAlgorithmType::PointClip);
            app->logger.log("Rectangle-Point Clip: click top-left and bottom-right of window, then the point to be clipped.");
            break;

        case IDM_CLIPPING_RECTANGLE_LINE:
            app->menu.setMode(DrawingMode::Clip);
            app->menu.setClippingType(ClippingType::Rectangle);
            app->menu.setClipAlgorithm(ClipAlgorithmType::LineClip);
            app->logger.log("Rectangle-Line Clip: click top-left and bottom-right of window, then line start and end of the line to be clipped.");
            break;

        case IDM_CLIPPING_RECTANGLE_POLYGON:
            app->menu.setMode(DrawingMode::Clip);
            app->menu.setClippingType(ClippingType::Rectangle);
            app->menu.setClipAlgorithm(ClipAlgorithmType::PolygonClip);
            app->logger.log("Rectangle-Polygon Clip: click top-left and bottom-right of window, then polygon vertices. Double-click to finish.");
            break;

        case IDM_CLIPPING_SQUARE_POINT:
            app->menu.setMode(DrawingMode::Clip);
            app->menu.setClippingType(ClippingType::Square);
            app->menu.setClipAlgorithm(ClipAlgorithmType::PointClip);
            app->logger.log("Square-Point Clip: click top-left and another point for the side length of window, then the point to be clipped.");
            break;

        case IDM_CLIPPING_SQUARE_LINE:
            app->menu.setMode(DrawingMode::Clip);
            app->menu.setClippingType(ClippingType::Square);
            app->menu.setClipAlgorithm(ClipAlgorithmType::LineClip);
            app->logger.log("Square-Line Clip: click top-left and another point for the side length of window, then line start and end of the line to be clipped.");
            break;

        case IDM_CLIPPING_CIRCLE_POINT:
            app->menu.setMode(DrawingMode::Clip);
            app->menu.setClippingType(ClippingType::Circle);
            app->menu.setClipAlgorithm(ClipAlgorithmType::PointClip);
            app->logger.log("Circle-Point Clip: click center and a point on the boundary of circle window, then the point to be clipped.");
            break;

        case IDM_CLIPPING_CIRCLE_LINE:
            app->menu.setMode(DrawingMode::Clip);
            app->menu.setClippingType(ClippingType::Circle);
            app->menu.setClipAlgorithm(ClipAlgorithmType::LineClip);
            app->logger.log("Circle-Line Clip: click center and a point on the boundary of circle window, then line start and end of the line to be clipped.");
            break;

        case IDM_SMILEY_FACE_HAPPY:
            app->addSmileyFace(true);
            break;

        case IDM_SMILEY_FACE_SAD:
            app->addSmileyFace(false);
            break;

        default:
            // Handle your own custom menu commands here, or log unhandled ones for debugging.
            break;
    }
}
