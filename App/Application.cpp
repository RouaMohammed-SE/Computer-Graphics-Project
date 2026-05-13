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

        // Filling Menu
        case IDM_FILL_CIRCLE_WITH_LINES:
            app->menu.setMode(DrawingMode::Fill);
            app->menu.setFillAlgorithm(FillAlgorithmType::CircleFillWithLines);
            app->logger.log("Fill Circle with lines: click center, then edge.");
            break;

        case IDM_FILL_CIRCLE_WITH_CIRCLES:
            app->menu.setMode(DrawingMode::Fill);
            app->menu.setFillAlgorithm(FillAlgorithmType::CircleFillWithCircles);
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
