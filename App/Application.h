#ifndef APP_APPLICATION_H
#define APP_APPLICATION_H

#include "../Core/InputHandler.h"
#include "../Core/Preferences.h"
#include "../Core/Window.h"
#include "../FileSystem/FileManager.h"
#include "../Menu/Menu.h"
#include "../Shapes/Shape.h"
#include "../Utils/Color.h"
#include <vector>

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

private:
    static void handlePaint(HDC hdc, void* context);
    static void handleMouseClick(const Point& position, void* context);
    static void handleMouseMove(const Point& position, void* context);

    Window window;
    InputHandler inputHandler;
    Menu menu;
    FileManager fileManager;
    Preferences preferences;
    std::vector<Shape*> shapes;
    Color drawingColor;
    Color backgroundColor;
};

#endif
