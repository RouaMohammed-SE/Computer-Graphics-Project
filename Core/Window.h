#pragma once
#ifndef CORE_WINDOW_H
#define CORE_WINDOW_H

#include "../Utils/Color.h"
#include "../Utils/Point.h"
#include "Enums.h"
#include <windows.h>
#include <vector>
#include <string>

class Window;

typedef void (*PaintCallback)(HDC hdc, void* context);
typedef void (*MouseCallback)(const Point& position, void* context);
typedef void (*RightClickCallback)(const Point& position, void* context);
typedef void (*CommandCallback)(int commandId, void* context);

// -----------------------------------------------------------------------
//  Layout constants
// -----------------------------------------------------------------------
static const int SIDEBAR_WIDTH    = 220;   // left tool panel
static const int TOOLBAR_HEIGHT   = 50;    // top icon toolbar
static const int PALETTE_HEIGHT   = 48;    // bottom colour bar
static const int STATUSBAR_HEIGHT = 24;    // bottom status strip

// -----------------------------------------------------------------------
//  Palette colours shown in the bottom bar (add/remove freely)
// -----------------------------------------------------------------------
static const COLORREF PALETTE_COLORS[] = {
    RGB(0,   0,   0),    // Black
    RGB(255, 255, 255),  // White
    RGB(220, 50,  50),   // Red
    RGB(50,  120, 220),  // Blue
    RGB(30,  160, 100),  // Green
    RGB(230, 160, 30),   // Amber
    RGB(130, 100, 230),  // Purple
    RGB(210, 80,  130),  // Pink
    RGB(220, 100, 40),   // Orange
    RGB(100, 160, 30),   // Lime
    RGB(130, 130, 130),  // Gray
    RGB(80,  50,  20),   // Brown
};
static const int PALETTE_COUNT = sizeof(PALETTE_COLORS) / sizeof(PALETTE_COLORS[0]);
static const int SWATCH_SIZE   = 22;
static const int SWATCH_GAP    = 4;

// -----------------------------------------------------------------------
//  Theme colour structures
// -----------------------------------------------------------------------
struct ThemeColors {
    COLORREF bg;           // main window / canvas bg
    COLORREF sidebar;      // sidebar background
    COLORREF toolbar;      // top toolbar background
    COLORREF palette;      // bottom palette bar background
    COLORREF statusBar;    // status bar background
    COLORREF border;       // separator lines
    COLORREF text;         // normal text
    COLORREF textMuted;    // secondary text
    COLORREF accent;       // highlight / active colour
    COLORREF itemHover;    // sidebar item hover
    COLORREF itemActive;   // sidebar item active background
    COLORREF sectionLabel; // section header text
};

// Owns the drawing surface and window-level rendering operations.
class Window {
public:
    Window();
    Window(int width, int height, const Color& backgroundColor);

    bool create(const wchar_t* title = L"Draw Studio");
    int  runMessageLoop();
    void clear();
    void refresh();

    void setBackgroundColor(const Color& color);
    void setCursor(HCURSOR hCursor);
    void setTheme(ThemeMode theme);
    void toggleTheme();
    ThemeMode getTheme() const { return currentTheme; }

    // Status bar text (shown bottom-left)
    void setStatusText(const std::wstring& text);
    // Mode label (shown bottom-right of palette bar)
    void setModeText(const std::wstring& text);

    void setPaintCallback(PaintCallback callback, void* context);
    void setMouseClickCallback(MouseCallback callback, void* context);
    void setMouseMoveCallback(MouseCallback callback, void* context);
    void setRightClickCallback(RightClickCallback callback, void* context);
    void setCommandCallback(CommandCallback callback, void* context);

    HWND getHandle() const;
    HWND getCanvasHandle() const { return hCanvas; }


    // Returns the client rect of the CANVAS only (excluding sidebar/bars).
    RECT getCanvasRect() const;

    // Converts a screen point to canvas-relative coordinates.
    Point screenToCanvas(int screenX, int screenY) const;

private:
    // ----- WinAPI plumbing -----
    static LRESULT CALLBACK windowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK sidebarProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK toolbarProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK paletteProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK statusProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    static LRESULT CALLBACK canvasProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);


    LRESULT handleMessage(UINT msg, WPARAM wp, LPARAM lp);
    LRESULT handleSidebar(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    LRESULT handleToolbar(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    LRESULT handlePaletteBar(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    LRESULT handleStatusBar(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

    // ----- Layout helpers -----
    void createChildWindows();
    void layoutChildren();          // called on WM_SIZE
    void repaintAll();

    // ----- Drawing helpers -----
    ThemeColors themeFor(ThemeMode t) const;
    void paintSidebar(HDC hdc, HWND hwnd);
    void paintToolbar(HDC hdc, HWND hwnd);
    void paintPaletteBar(HDC hdc, HWND hwnd);
    void paintStatusBar(HDC hdc, HWND hwnd);
    void drawRoundRect(HDC hdc, RECT r, int rx, COLORREF fill, COLORREF border);
    void drawText(HDC hdc, const wchar_t* text, RECT r, COLORREF color, int size = 12, bool bold = false, DWORD flags = DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    COLORREF backgroundColorRef() const;

    // ----- Sidebar item hit-testing -----
    int  hitTestSidebar(int y) const;   // returns IDM_* or -1
    void setSidebarActiveItem(int idm);

    // ----- Data -----
    HINSTANCE instance;
    HWND handle;          // main window
    HWND hSidebar;        // left panel
    HWND hToolbar;        // top toolbar
    HWND hPaletteBar;     // bottom colour strip
    HWND hStatusBar;      // very bottom strip
    HWND hCanvas;         // the actual drawing surface

    int width, height;
    Color backgroundColor;
    HCURSOR currentCursor;
    ThemeMode currentTheme;

    std::wstring statusText;
    std::wstring modeText;

    int  activePaletteIndex;  // currently selected swatch
    int  activeSidebarItem;   // IDM_* of active tool
    int  sidebarScrollY;      // current vertical scroll offset (pixels)

    PaintCallback     paintCallback;
    MouseCallback     mouseClickCallback;
    MouseCallback     mouseMoveCallback;
    RightClickCallback rightClickCallback;
    CommandCallback   commandCallback;

    void* paintContext;
    void* mouseClickContext;
    void* mouseMoveContext;
    void* rightClickContext;
    void* commandContext;

    // Sidebar section/item layout (built once, reused for hit-testing & painting)
    struct SidebarItem {
        int   idm;          // IDM_* command, or -1 for section label
        bool  isSection;
        std::wstring label;
        std::wstring iconText;  // single Segoe UI Symbol / Webdings char, or empty
        RECT  rect;         // filled by paintSidebar / layout
    };
    std::vector<SidebarItem> sidebarItems;
    void buildSidebarItems();

    // Toolbar button layout
    struct ToolbarButton {
        int   idm;
        std::wstring label;
        std::wstring icon;
        RECT  rect;
    };
    std::vector<ToolbarButton> toolbarButtons;
    void buildToolbarButtons();

    // Stored fonts (created lazily, destroyed with window)
    HFONT fontNormal;
    HFONT fontBold;
    HFONT fontSmall;
    HFONT fontSection;
    void  createFonts();
    void  destroyFonts();
};

#endif