#pragma once
#ifndef CORE_PREFERENCES_H
#define CORE_PREFERENCES_H

#include "Enums.h"
#include "../Utils/Color.h"

// Stores user preferences such as colors and mouse cursor style.
class Preferences {
public:
    Preferences();

    void setBackgroundColor(const Color& color);
    void setDrawingColor(const Color& color);
    void setMouseCursor(MouseCursorType cursorType);

    Color getBackgroundColor() const;
    Color getDrawingColor() const;
    MouseCursorType getMouseCursor() const;

private:
    Color backgroundColor;
    Color drawingColor;
    MouseCursorType mouseCursor;
};

#endif
