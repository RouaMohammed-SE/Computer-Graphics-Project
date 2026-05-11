#ifndef CORE_WINDOW_H
#define CORE_WINDOW_H

#include "../Utils/Color.h"
#include "../Utils/Point.h"
#include <windows.h>

class Window;

typedef void (*PaintCallback)(HDC hdc, void* context);
typedef void (*MouseCallback)(const Point& position, void* context);

// Owns the drawing surface and window-level rendering operations.
class Window {
public:
    Window();
    Window(int width, int height, const Color& backgroundColor);

    bool create(const char* title = "Computer Graphics Project");
    int runMessageLoop();
    void clear();
    void refresh();
    void setBackgroundColor(const Color& color);
    void setPaintCallback(PaintCallback callback, void* context);
    void setMouseClickCallback(MouseCallback callback, void* context);
    void setMouseMoveCallback(MouseCallback callback, void* context);
    HWND getHandle() const;

private:
    static LRESULT CALLBACK windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(UINT message, WPARAM wParam, LPARAM lParam);
    COLORREF backgroundColorRef() const;

    HINSTANCE instance;
    HWND handle;
    int width;
    int height;
    Color backgroundColor;
    PaintCallback paintCallback;
    MouseCallback mouseClickCallback;
    MouseCallback mouseMoveCallback;
    void* paintContext;
    void* mouseClickContext;
    void* mouseMoveContext;
};

#endif
