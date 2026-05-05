#ifndef CORE_WINDOW_H
#define CORE_WINDOW_H

#include "../Utils/Color.h"

// Owns the drawing surface and window-level rendering operations.
class Window {
public:
    Window();
    Window(int width, int height, const Color& backgroundColor);

    void create();
    void clear();
    void refresh();
    void setBackgroundColor(const Color& color);

private:
    int width;
    int height;
    Color backgroundColor;
};

#endif
