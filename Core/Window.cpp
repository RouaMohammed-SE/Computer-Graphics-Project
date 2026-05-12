#include "Window.h"
#include <windowsx.h>
#include "../Menu/MenuCommands.h"

Window::Window()
    : instance(GetModuleHandle(nullptr)),
      handle(nullptr),
      width(800),
      height(600),
      backgroundColor(255, 255, 255),
      currentCursor(nullptr),
      paintCallback(nullptr),
      mouseClickCallback(nullptr),
      mouseMoveCallback(nullptr),
      commandCallback(nullptr),
      paintContext(nullptr),
      mouseClickContext(nullptr),
      mouseMoveContext(nullptr),
      commandContext(nullptr) {}

Window::Window(int width, int height, const Color& backgroundColor)
    : instance(GetModuleHandle(nullptr)),
      handle(nullptr),
      width(width),
      height(height),
      backgroundColor(backgroundColor),
      currentCursor(nullptr),
      paintCallback(nullptr),
      mouseClickCallback(nullptr),
      mouseMoveCallback(nullptr),
        commandCallback(nullptr),
      paintContext(nullptr),
      mouseClickContext(nullptr),
      mouseMoveContext(nullptr),
      commandContext(nullptr) {}

bool Window::create(const wchar_t* title) { 
    // Register window class, builds the menu bar, and creats the HWND
    WNDCLASSEX wc   = {};
    wc.cbSize       = sizeof(WNDCLASSEX);
    wc.style        = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc  = Window::windowProcedure;
    wc.hInstance    = instance;
    wc.hCursor      = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(backgroundColorRef());
    wc.lpszClassName = L"CGWindowClass";

    if (!RegisterClassEx(&wc)) return false;

    // ── File Menu
    HMENU hFileMenu = CreatePopupMenu();
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_CLEAR, L"Clear Screen");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE,  L"Save...");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_LOAD,  L"Load...");

    // ── Preferences Menu
    HMENU hCursorMenu = CreatePopupMenu();
    AppendMenu(hCursorMenu, MF_STRING, IDM_PREF_CURSOR_DEFAULT,    L"Default");
    AppendMenu(hCursorMenu, MF_STRING, IDM_PREF_CURSOR_CROSSHAIR,  L"Crosshair");
    AppendMenu(hCursorMenu, MF_STRING, IDM_PREF_CURSOR_HAND,       L"Hand");

    HMENU hPrefMenu = CreatePopupMenu();
    AppendMenu(hPrefMenu, MF_STRING, IDM_PREF_BG_WHITE, L"Background: White");
    AppendMenu(hPrefMenu, MF_POPUP, (UINT_PTR)hCursorMenu, L"Mouse Cursor");
    AppendMenu(hPrefMenu, MF_STRING, IDM_PREF_COLOR, L"Shape Color...");

        // ── Menu Bar ───────────────────────────────────────────────
    HMENU hMenuBar = CreateMenu();
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hPrefMenu, L"Preferences");
    // APPEND YOUR OWN MENUS HERE (LINES, CIRCLES, ETC.)

    // Pass 'this' as lpCreateParams so WM_NCCREATE can store the pointer
    handle = CreateWindowEx(
        0, L"CGWindowClass", title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, hMenuBar, instance, this
    );

    if (!handle) return false;

    ShowWindow(handle, SW_SHOW);
    UpdateWindow(handle);
    return true;
}

int Window::runMessageLoop() {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

void Window::clear() {
    if (!handle) return;
    HDC  hdc  = GetDC(handle);
    RECT rect;
    GetClientRect(handle, &rect);
    HBRUSH brush = CreateSolidBrush(backgroundColorRef());
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);
    ReleaseDC(handle, hdc);
}

void Window::refresh() {
    // Marks the window dirty so WM_PAINT fires and all shapes are redrawn.
    if (handle) InvalidateRect(handle, NULL, TRUE);
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

void Window::setCommandCallback(CommandCallback callback, void* context) {
    commandCallback = callback;
    commandContext = context;
}

HWND Window::getHandle() const {
    return handle;
}

// Static WinAPI callback. On WM_NCCREATE we store 'this' so every subsequent
// message can be routed to the right Window instance's handleMessage().
LRESULT CALLBACK Window::windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    Window* self = nullptr;

    if (message == WM_NCCREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        self = reinterpret_cast<Window*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->handle = hwnd;
    } else {
        self = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (self) return self->handleMessage(message, wParam, lParam);
    return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT Window::handleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(handle, &ps);
        if (paintCallback) paintCallback(hdc, paintContext);
        EndPaint(handle, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        if (mouseClickCallback) {
            Point pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            mouseClickCallback(pos, mouseClickContext);
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (mouseMoveCallback) {
            Point pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            mouseMoveCallback(pos, mouseMoveContext);
        }
        return 0;
    }

    // WM_COMMAND fires when the user clicks any menu item.
    // LOWORD(wParam) is the IDM_* constant we defined in MenuCommands.h.
    case WM_COMMAND: {
        if (commandCallback) commandCallback(LOWORD(wParam), commandContext);
        return 0;
    }

    // Intercept every cursor-reset so our custom cursor choice persists.
    case WM_SETCURSOR: {
        if (LOWORD(lParam) == HTCLIENT && currentCursor != NULL) {
            SetCursor(currentCursor);
            return TRUE;
        }
        return DefWindowProc(handle, message, wParam, lParam);
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(handle, message, wParam, lParam);
    }
}

COLORREF Window::backgroundColorRef() const {
    return RGB(backgroundColor.r, backgroundColor.g, backgroundColor.b);
}
