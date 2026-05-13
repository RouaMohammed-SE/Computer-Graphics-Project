#pragma once
#ifndef APP_APPLICATION_H
#define APP_APPLICATION_H

#include "../Core/InputHandler.h"
#include "../Core/Preferences.h"
#include "../Core/Window.h"
#include "../FileSystem/FileManager.h"
#include "../Menu/Menu.h"
#include "../Shapes/Shape.h"
#include "../Utils/Color.h"
#include "../Utils/Logger.h"
#include "../Utils/Point.h"
#include <windows.h>
#include <vector>

enum class PersistentDrawingType {
    CircleFillWithLines,
    CircleFillWithCircles,
    RectangleFillWithCurves,
    SquareFillWithCurves,
    CircleClipPoint,
    CircleClipLine
};

struct PersistentDrawing {
    PersistentDrawingType type;
    std::vector<Point> points;
    int radius;
    int innerRadius;
    int outerRadius;
    int quarter;
    int sideLength;
    Color color;
};

// Main controller that coordinates window, input, menu, files, and shapes.
class Application {
public:
    Application();
    ~Application();

    void run();
    void update();
    void render(HDC hdc);
    void addShape(Shape* shape);
    void clearShapes();
    void setDrawingColor(const Color& color);
    void setBackgroundColor(const Color& color);

    Color getDrawingColor() const { return drawingColor; }

private:
    static void handlePaint(HDC hdc, void* context);
    static void handleMouseClick(const Point& position, void* context);
    static void handleMouseMove(const Point& position, void* context);
    // Called by Window whenever a menu item is clicked (WM_COMMAND)
    static void handleCommand    (int commandId, void* context);

    void addSmileyFace(const Point& center, bool happy);
    void resetPendingClicks();
    void replayPersistentDrawing(HDC hdc, const PersistentDrawing& drawing);

    Window window;
    InputHandler inputHandler;
    Menu menu;
    FileManager fileManager;
    Preferences preferences;
    Logger logger;

    std::vector<Shape*> shapes;
    std::vector<PersistentDrawing> persistentDrawings;
    std::vector<Point> pendingClicks;
    int fillQuarter;
    bool pendingHappyFace;
    Color drawingColor;
    Color backgroundColor;
};

#endif
