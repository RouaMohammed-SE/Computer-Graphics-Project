#include "Preferences.h"

Preferences::Preferences()
    : backgroundColor(), drawingColor(), mouseCursor(MouseCursorType::Default) {
    // TODO: Add initialization logic if needed.
}

void Preferences::setBackgroundColor(const Color& color) {
    // TODO: Store background color preference.
}

void Preferences::setDrawingColor(const Color& color) {
    // TODO: Store drawing color preference.
}

void Preferences::setMouseCursor(MouseCursorType cursorType) {
    // TODO: Store mouse cursor preference.
}

Color Preferences::getBackgroundColor() const {
    // TODO: Return background color preference.
    return Color();
}

Color Preferences::getDrawingColor() const {
    // TODO: Return drawing color preference.
    return Color();
}

MouseCursorType Preferences::getMouseCursor() const {
    // TODO: Return mouse cursor preference.
    return MouseCursorType::Default;
}
