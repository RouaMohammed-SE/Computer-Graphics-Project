#pragma once
#ifndef CORE_PREFERENCES_H
#define CORE_PREFERENCES_H

#include "Enums.h"
#include "../Utils/Color.h"

// Stores user preferences such as colors, mouse cursor style, and theme.
class Preferences {
public:
    Preferences();

    void setBackgroundColor(const Color& color);
    void setDrawingColor(const Color& color);
    void setMouseCursor(MouseCursorType cursorType);
    void setTheme(ThemeMode theme);

    Color getBackgroundColor() const;
    Color getDrawingColor() const;
    MouseCursorType getMouseCursor() const;
    ThemeMode getTheme() const;
    bool isDarkMode() const;

private:
    Color backgroundColor;
    Color drawingColor;
    MouseCursorType mouseCursor;
    ThemeMode theme;
};

#endif