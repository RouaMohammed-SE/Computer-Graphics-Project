#include "Application.h"

Application::Application()
    : window(800, 600, Color(255, 255, 255)),
      inputHandler(),
      menu(),
      fileManager(),
      preferences(),
      shapes(),
      drawingColor(0, 0, 0),
      backgroundColor(255, 255, 255) {
    window.setPaintCallback(Application::handlePaint, this);
    window.setMouseClickCallback(Application::handleMouseClick, this);
    window.setMouseMoveCallback(Application::handleMouseMove, this);
}

Application::~Application() {
    // TODO: Release owned shapes.
}

void Application::run() {
    // TODO: Create the WinAPI window and start the message loop.
}

void Application::update() {
    // TODO: Process input and menu state.
}

void Application::render(HDC hdc) {
    // TODO: Draw all stored shapes using the provided WinAPI HDC.
}

void Application::addShape(Shape* shape) {
    // TODO: Add shape to collection.
}

void Application::clearShapes() {
    // TODO: Clear shape collection.
}

void Application::setDrawingColor(const Color& color) {
    drawingColor = color;
}

void Application::setBackgroundColor(const Color& color) {
    backgroundColor = color;
    window.setBackgroundColor(color);
}

void Application::handlePaint(HDC hdc, void* context) {
    // TODO: Forward WinAPI paint events to the application renderer.
}

void Application::handleMouseClick(const Point& position, void* context) {
    // TODO: Forward WinAPI mouse clicks to the input handler.
}

void Application::handleMouseMove(const Point& position, void* context) {
    // TODO: Forward WinAPI mouse movement to the input handler.
}
