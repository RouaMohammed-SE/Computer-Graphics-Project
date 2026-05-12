#ifndef CORE_WINDOW_H
#define CORE_WINDOW_H

#include "../Utils/Color.h"
#include "../Utils/Point.h"
#include <windows.h>

class Window;

typedef void (*PaintCallback)(HDC hdc, void* context);
typedef void (*MouseCallback)(const Point& position, void* context);
typedef void (*CommandCallback)(int commandId, void* context);

// Owns the drawing surface and window-level rendering operations.
class Window {
public:
    Window();
    Window(int width, int height, const Color& backgroundColor);

    bool create(const wchar_t* title = L"Computer Graphics Project");
    int runMessageLoop();
    void clear();
    void refresh();
    void setBackgroundColor(const Color& color);
    void setCursor(HCURSOR hCursor); 

    void setPaintCallback(PaintCallback callback, void* context);
    void setMouseClickCallback(MouseCallback callback, void* context);
    void setMouseMoveCallback(MouseCallback callback, void* context);
    void setCommandCallback(CommandCallback callback, void* context);
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
    HCURSOR currentCursor;

    PaintCallback paintCallback;
    MouseCallback mouseClickCallback;
    MouseCallback mouseMoveCallback;
    CommandCallback commandCallback;

    void* paintContext;
    void* mouseClickContext;
    void* mouseMoveContext;
    void* commandContext;
};

#endif
