#include "Preferences.h"

Preferences::Preferences()
    : backgroundColor(255, 255, 255),
      drawingColor(0, 0, 0),
      mouseCursor(MouseCursorType::Default) {}

void Preferences::setBackgroundColor(const Color& color) {
    // Store background color preference.
    backgroundColor = color;
}

void Preferences::setDrawingColor(const Color& color) {
    // Store drawing color preference.
    drawingColor = color;
}

void Preferences::setMouseCursor(MouseCursorType cursorType) {
    // Store mouse cursor preference.
    mouseCursor = cursorType;
}

Color Preferences::getBackgroundColor() const {
    // Return background color preference.
    return backgroundColor;
}

Color Preferences::getDrawingColor() const {
    // Return drawing color preference.
    return drawingColor;
}

MouseCursorType Preferences::getMouseCursor() const {
    // Return mouse cursor preference.
    return mouseCursor;
}
