#include "Application.h"

Application::Application()
    : window(),
      inputHandler(),
      menu(),
      fileManager(),
      preferences(),
      shapes(),
      drawingColor(),
      backgroundColor() {
    // TODO: Add initialization logic if needed.
}

Application::~Application() {
    // TODO: Release owned shapes.
}

void Application::run() {
    // TODO: Start application loop.
}

void Application::update() {
    // TODO: Process input and menu state.
}

void Application::render() {
    // TODO: Draw all stored shapes.
}

void Application::addShape(Shape* shape) {
    // TODO: Add shape to collection.
}

void Application::clearShapes() {
    // TODO: Clear shape collection.
}

void Application::setDrawingColor(const Color& color) {
    // TODO: Set active drawing color.
}

void Application::setBackgroundColor(const Color& color) {
    // TODO: Set active background color.
}
