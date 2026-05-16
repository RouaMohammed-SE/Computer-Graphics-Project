#include "Preferences.h"

Preferences::Preferences()
    : backgroundColor(255, 255, 255),
      drawingColor(0, 0, 0),
      mouseCursor(MouseCursorType::Default),
      theme(ThemeMode::Dark) {}

void Preferences::setBackgroundColor(const Color& color) {
    backgroundColor = color;
}

void Preferences::setDrawingColor(const Color& color) {
    drawingColor = color;
}

void Preferences::setMouseCursor(MouseCursorType cursorType) {
    mouseCursor = cursorType;
}

void Preferences::setTheme(ThemeMode t) {
    theme = t;
}

Color Preferences::getBackgroundColor() const {
    return backgroundColor;
}

Color Preferences::getDrawingColor() const {
    return drawingColor;
}

MouseCursorType Preferences::getMouseCursor() const {
    return mouseCursor;
}

ThemeMode Preferences::getTheme() const {
    return theme;
}

bool Preferences::isDarkMode() const {
    return theme == ThemeMode::Dark;
}