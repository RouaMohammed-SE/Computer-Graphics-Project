#include "Window.h"
#include <windowsx.h>

Window::Window()
    : instance(GetModuleHandle(nullptr)),
      handle(nullptr),
      width(800),
      height(600),
      backgroundColor(255, 255, 255),
      paintCallback(nullptr),
      mouseClickCallback(nullptr),
      mouseMoveCallback(nullptr),
      paintContext(nullptr),
      mouseClickContext(nullptr),
      mouseMoveContext(nullptr) {}

Window::Window(int width, int height, const Color& backgroundColor)
    : instance(GetModuleHandle(nullptr)),
      handle(nullptr),
      width(width),
      height(height),
      backgroundColor(backgroundColor),
      paintCallback(nullptr),
      mouseClickCallback(nullptr),
      mouseMoveCallback(nullptr),
      paintContext(nullptr),
      mouseClickContext(nullptr),
      mouseMoveContext(nullptr) {}

bool Window::create(const char* title) {
    // TODO: Register a WinAPI window class and create the HWND.
    return false;
}

int Window::runMessageLoop() {
    // TODO: Run the WinAPI message loop.
    return 0;
}

void Window::clear() {
    // TODO: Clear the WinAPI drawing surface.
}

void Window::refresh() {
    // TODO: Invalidate/update the WinAPI window.
}

void Window::setBackgroundColor(const Color& color) {
    backgroundColor = color;
}

void Window::setPaintCallback(PaintCallback callback, void* context) {
    paintCallback = callback;
    paintContext = context;
}

void Window::setMouseClickCallback(MouseCallback callback, void* context) {
    mouseClickCallback = callback;
    mouseClickContext = context;
}

void Window::setMouseMoveCallback(MouseCallback callback, void* context) {
    mouseMoveCallback = callback;
    mouseMoveContext = context;
}

HWND Window::getHandle() const {
    return handle;
}

LRESULT CALLBACK Window::windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // TODO: Route WinAPI messages to the matching Window instance.
    return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT Window::handleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    // TODO: Handle WM_PAINT, mouse messages, and WM_DESTROY.
    return DefWindowProc(handle, message, wParam, lParam);
}

COLORREF Window::backgroundColorRef() const {
    return RGB(backgroundColor.r, backgroundColor.g, backgroundColor.b);
}
