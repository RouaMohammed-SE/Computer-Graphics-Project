#include "Window.h"
#include <windowsx.h>
#include "../Menu/MenuCommands.h"
#include <string>

// ============================================================
//  Theme definitions
// ============================================================
ThemeColors Window::themeFor(ThemeMode t) const {
    ThemeColors c;
    if (t == ThemeMode::Dark) {
        c.bg           = RGB(26,  26,  46);
        c.sidebar      = RGB(22,  22,  40);
        c.toolbar      = RGB(30,  30,  55);
        c.palette      = RGB(22,  22,  40);
        c.statusBar    = RGB(15,  15,  30);
        c.border       = RGB(50,  50,  80);
        c.text         = RGB(220, 220, 240);
        c.textMuted    = RGB(120, 120, 160);
        c.accent       = RGB(0,   212, 170);
        c.itemHover    = RGB(40,  40,  70);
        c.itemActive   = RGB(0,   60,  50);
        c.sectionLabel = RGB(0,   212, 170);
    } else {
        c.bg           = RGB(240, 241, 248);
        c.sidebar      = RGB(255, 255, 255);
        c.toolbar      = RGB(248, 249, 252);
        c.palette      = RGB(255, 255, 255);
        c.statusBar    = RGB(230, 232, 242);
        c.border       = RGB(210, 212, 228);
        c.text         = RGB(30,  30,  60);
        c.textMuted    = RGB(100, 100, 140);
        c.accent       = RGB(10,  143, 115);
        c.itemHover    = RGB(235, 240, 250);
        c.itemActive   = RGB(210, 242, 235);
        c.sectionLabel = RGB(10,  143, 115);
    }
    return c;
}

// ============================================================
//  Constructor helpers
// ============================================================
Window::Window()
    : instance(GetModuleHandle(nullptr)), handle(nullptr),
      hSidebar(nullptr), hToolbar(nullptr), hPaletteBar(nullptr),
      hStatusBar(nullptr), hCanvas(nullptr),
      width(1100), height(700),
      backgroundColor(255,255,255),
      currentCursor(nullptr),
      currentTheme(ThemeMode::Dark),
      statusText(L"Ready"),
      modeText(L"No tool selected"),
      activePaletteIndex(0),
      activeSidebarItem(-1),
      sidebarScrollY(0),
      paintCallback(nullptr), mouseClickCallback(nullptr),
      mouseMoveCallback(nullptr), rightClickCallback(nullptr),
      commandCallback(nullptr),
      paintContext(nullptr), mouseClickContext(nullptr),
      mouseMoveContext(nullptr), rightClickContext(nullptr),
      commandContext(nullptr),
      fontNormal(nullptr), fontBold(nullptr),
      fontSmall(nullptr), fontSection(nullptr) {}

Window::Window(int w, int h, const Color& bg)
    : Window() { width = w; height = h; backgroundColor = bg; }

// ============================================================
//  Font helpers
// ============================================================
void Window::createFonts() {
    fontNormal  = CreateFont(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    fontBold    = CreateFont(-12, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    fontSmall   = CreateFont(-10, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    fontSection = CreateFont(-10, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
}

void Window::destroyFonts() {
    if (fontNormal)  { DeleteObject(fontNormal);  fontNormal  = nullptr; }
    if (fontBold)    { DeleteObject(fontBold);    fontBold    = nullptr; }
    if (fontSmall)   { DeleteObject(fontSmall);   fontSmall   = nullptr; }
    if (fontSection) { DeleteObject(fontSection); fontSection = nullptr; }
}

// ============================================================
//  Sidebar items definition
// ============================================================
void Window::buildSidebarItems() {
    sidebarItems.clear();
    auto sec = [&](const wchar_t* label) {
        SidebarItem s; s.idm = -1; s.isSection = true; s.label = label; sidebarItems.push_back(s);
    };
    auto item = [&](int idm, const wchar_t* label) {
        SidebarItem s; s.idm = idm; s.isSection = false; s.label = label; sidebarItems.push_back(s);
    };

    sec(L"FILE");
    item(IDM_FILE_CLEAR, L"  Clear Canvas");
    item(IDM_FILE_SAVE,  L"  Save Project");
    item(IDM_FILE_LOAD,  L"  Load Project");

    sec(L"PREFERENCES");
    item(IDM_PREF_BG_WHITE,        L"  White Background");
    item(IDM_PREF_CURSOR_DEFAULT,  L"  Cursor: Default");
    item(IDM_PREF_CURSOR_CROSSHAIR,L"  Cursor: Crosshair");
    item(IDM_PREF_CURSOR_HAND,     L"  Cursor: Hand");
    item(IDM_PREF_COLOR,           L"  Shape Colour...");

    sec(L"LINES");
    item(IDM_DDA_LINE,        L"  DDA Algorithm");
    item(IDM_MIDPOINT_LINE,   L"  Midpoint Line");
    item(IDM_PARAMETRIC_LINE, L"  Parametric Line");

    sec(L"CIRCLES");
    item(IDM_DIRECT_CIRCLE,            L"  Direct");
    item(IDM_POLAR_CIRCLE,             L"  Polar");
    item(IDM_ITERATIVE_POLAR_CIRCLE,   L"  Iterative Polar");
    item(IDM_MIDPOINT_CIRCLE,          L"  Midpoint");
    item(IDM_MODIFIED_MIDPOINT_CIRCLE, L"  Modified Midpoint");

    sec(L"ELLIPSES");
    item(IDM_DIRECT_ELLIPSE,          L"  Direct");
    item(IDM_POLAR_ELLIPSE,           L"  Polar");
    item(IDM_ITERATIVE_POLAR_ELLIPSE, L"  Iterative Polar");
    item(IDM_MIDPOINT_ELLIPSE,        L"  Midpoint");

    sec(L"CURVES");
    item(IDM_CARDINAL_SPLINE_CURVE, L"  Cardinal Spline");

    sec(L"FILLING");
    item(IDM_FILL_CIRCLE_WITH_LINES,   L"  Circle w/ Lines");
    item(IDM_FILL_CIRCLE_WITH_CIRCLES, L"  Circle w/ Circles");
    item(IDM_FILL_SQUARE_CURVES,       L"  Square w/ Hermite");
    item(IDM_FILL_RECTANGLE_CURVES,    L"  Rect w/ Bezier");
    item(IDM_FILL_CONVEX,              L"  Convex Fill");
    item(IDM_FILL_NON_CONVEX,          L"  Non-Convex Fill");
    item(IDM_FILL_FLOOD_RECURSIVE,     L"  Flood (Recursive)");
    item(IDM_FILL_FLOOD_NON_RECURSIVE, L"  Flood (Non-Rec)");

    sec(L"CLIPPING");
    item(IDM_CLIPPING_RECTANGLE_POINT,   L"  Rect \u00b7 Point");
    item(IDM_CLIPPING_RECTANGLE_LINE,    L"  Rect \u00b7 Line");
    item(IDM_CLIPPING_RECTANGLE_POLYGON, L"  Rect \u00b7 Polygon");
    item(IDM_CLIPPING_SQUARE_POINT,      L"  Square \u00b7 Point");
    item(IDM_CLIPPING_SQUARE_LINE,       L"  Square \u00b7 Line");
    item(IDM_CLIPPING_CIRCLE_POINT,      L"  Circle \u00b7 Point");
    item(IDM_CLIPPING_CIRCLE_LINE,       L"  Circle \u00b7 Line");

    sec(L"FACES");
    item(IDM_SMILEY_FACE_HAPPY, L"  Happy Face");
    item(IDM_SMILEY_FACE_SAD,   L"  Sad Face");
}

// ============================================================
//  Toolbar buttons definition
// ============================================================
void Window::buildToolbarButtons() {
    toolbarButtons.clear();
    // { idm, label, unused-icon-placeholder }
    // We keep label short so it fits in 52px buttons
    toolbarButtons.push_back({IDM_FILE_CLEAR,    L"Clear", L""});
    toolbarButtons.push_back({-2,                L"",      L""});   // separator
    toolbarButtons.push_back({IDM_DDA_LINE,      L"Line",  L""});
    toolbarButtons.push_back({IDM_MIDPOINT_CIRCLE,L"Circle",L""});
    toolbarButtons.push_back({IDM_MIDPOINT_ELLIPSE,L"Ellipse",L""});
    toolbarButtons.push_back({IDM_CARDINAL_SPLINE_CURVE,L"Curve",L""});
    toolbarButtons.push_back({IDM_FILL_FLOOD_RECURSIVE,L"Flood",L""});
    toolbarButtons.push_back({IDM_FILL_CONVEX,   L"Convex",L""});
    toolbarButtons.push_back({IDM_FILL_NON_CONVEX,L"N-Conv",L""});
    toolbarButtons.push_back({IDM_CLIPPING_RECTANGLE_POINT,L"R-Pt",L""});
    toolbarButtons.push_back({IDM_CLIPPING_RECTANGLE_LINE, L"R-Ln",L""});
    toolbarButtons.push_back({IDM_CLIPPING_RECTANGLE_POLYGON,L"R-Poly",L""});
    toolbarButtons.push_back({IDM_CLIPPING_SQUARE_LINE,    L"S-Ln",L""});
    toolbarButtons.push_back({IDM_CLIPPING_CIRCLE_POINT,   L"C-Pt",L""});
    toolbarButtons.push_back({IDM_CLIPPING_CIRCLE_LINE,    L"C-Ln",L""});
    toolbarButtons.push_back({IDM_SMILEY_FACE_HAPPY,       L"Happy",L""});
    toolbarButtons.push_back({IDM_SMILEY_FACE_SAD,         L"Sad",  L""});
    // Theme toggle — special ID
    toolbarButtons.push_back({-3, L"Theme", L""});
}

// ============================================================
//  create() — registers classes, makes main + child windows
// ============================================================
bool Window::create(const wchar_t* title) {
    createFonts();
    buildSidebarItems();
    buildToolbarButtons();

    // --- Register main window class ---
    WNDCLASSEX wc = {};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = Window::windowProcedure;
    wc.hInstance     = instance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszClassName = L"DS_MainWindow";
    if (!RegisterClassEx(&wc)) return false;

    // --- Register sidebar class ---
    WNDCLASSEX wcs = {};
    wcs.cbSize        = sizeof(WNDCLASSEX);
    wcs.style         = CS_HREDRAW | CS_VREDRAW;
    wcs.lpfnWndProc   = Window::sidebarProcedure;
    wcs.hInstance     = instance;
    wcs.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcs.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wcs.lpszClassName = L"DS_Sidebar";
    RegisterClassEx(&wcs);

    // --- Register toolbar class ---
    WNDCLASSEX wct = {};
    wct.cbSize        = sizeof(WNDCLASSEX);
    wct.style         = CS_HREDRAW | CS_VREDRAW;
    wct.lpfnWndProc   = Window::toolbarProcedure;
    wct.hInstance     = instance;
    wct.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wct.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wct.lpszClassName = L"DS_Toolbar";
    RegisterClassEx(&wct);

    // --- Register palette bar class ---
    WNDCLASSEX wcp = {};
    wcp.cbSize        = sizeof(WNDCLASSEX);
    wcp.style         = CS_HREDRAW | CS_VREDRAW;
    wcp.lpfnWndProc   = Window::paletteProcedure;
    wcp.hInstance     = instance;
    wcp.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcp.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wcp.lpszClassName = L"DS_Palette";
    RegisterClassEx(&wcp);

    // --- Register status bar class ---
    WNDCLASSEX wcst = {};
    wcst.cbSize        = sizeof(WNDCLASSEX);
    wcst.style         = CS_HREDRAW | CS_VREDRAW;
    wcst.lpfnWndProc   = Window::statusProcedure;
    wcst.hInstance     = instance;
    wcst.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcst.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wcst.lpszClassName = L"DS_Status";
    RegisterClassEx(&wcst);

    // --- Register canvas class ---
    WNDCLASSEX wcc = {};
    wcc.cbSize        = sizeof(WNDCLASSEX);
    wcc.style         = CS_HREDRAW | CS_VREDRAW;
    wcc.lpfnWndProc = Window::canvasProcedure;
    wcc.hInstance     = instance;
    wcc.hCursor       = LoadCursor(NULL, IDC_CROSS);
    wcc.hbrBackground = CreateSolidBrush(RGB(255,255,255));
    wcc.lpszClassName = L"DS_Canvas";
    RegisterClassEx(&wcc);

    // --- Create main window (no menu bar) ---
    handle = CreateWindowEx(
        0, L"DS_MainWindow", title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        NULL, NULL, instance, this
    );
    if (!handle) return false;

    createChildWindows();

    ShowWindow(handle, SW_SHOW);
    UpdateWindow(handle);
    return true;
}

void Window::createChildWindows() {
    ThemeColors tc = themeFor(currentTheme);

    // Toolbar (top)
    hToolbar = CreateWindowEx(0, L"DS_Toolbar", NULL,
        WS_CHILD | WS_VISIBLE,
        0, 0, width, TOOLBAR_HEIGHT,
        handle, NULL, instance, this);

    // Sidebar (left, below toolbar)
    int sidebarTop = TOOLBAR_HEIGHT;
    int sidebarHeight = height - TOOLBAR_HEIGHT - PALETTE_HEIGHT - STATUSBAR_HEIGHT;
    hSidebar = CreateWindowEx(0, L"DS_Sidebar", NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        0, sidebarTop, SIDEBAR_WIDTH, sidebarHeight,
        handle, NULL, instance, this);

    // Canvas (right of sidebar, below toolbar, above palette)
    int canvasLeft = SIDEBAR_WIDTH;
    int canvasTop  = TOOLBAR_HEIGHT;
    int canvasW    = width  - SIDEBAR_WIDTH;
    int canvasH    = height - TOOLBAR_HEIGHT - PALETTE_HEIGHT - STATUSBAR_HEIGHT;
    hCanvas = CreateWindowEx(0, L"DS_Canvas", NULL,
        WS_CHILD | WS_VISIBLE,
        canvasLeft, canvasTop, canvasW, canvasH,
        handle, NULL, instance, this);

    // Palette bar (bottom, above status bar)
    int paletteTop = height - PALETTE_HEIGHT - STATUSBAR_HEIGHT;
    hPaletteBar = CreateWindowEx(0, L"DS_Palette", NULL,
        WS_CHILD | WS_VISIBLE,
        0, paletteTop, width, PALETTE_HEIGHT,
        handle, NULL, instance, this);

    // Status bar (very bottom)
    int statusTop = height - STATUSBAR_HEIGHT;
    hStatusBar = CreateWindowEx(0, L"DS_Status", NULL,
        WS_CHILD | WS_VISIBLE,
        0, statusTop, width, STATUSBAR_HEIGHT,
        handle, NULL, instance, this);
}

// ============================================================
//  layoutChildren — called on WM_SIZE
// ============================================================
void Window::layoutChildren() {
    RECT rc;
    GetClientRect(handle, &rc);
    int W = rc.right;
    int H = rc.bottom;

    if (hToolbar)    SetWindowPos(hToolbar,    NULL, 0, 0, W, TOOLBAR_HEIGHT, SWP_NOZORDER);
    if (hSidebar)    SetWindowPos(hSidebar,    NULL, 0, TOOLBAR_HEIGHT, SIDEBAR_WIDTH, H - TOOLBAR_HEIGHT - PALETTE_HEIGHT - STATUSBAR_HEIGHT, SWP_NOZORDER);
    if (hCanvas)     SetWindowPos(hCanvas,     NULL, SIDEBAR_WIDTH, TOOLBAR_HEIGHT, W - SIDEBAR_WIDTH, H - TOOLBAR_HEIGHT - PALETTE_HEIGHT - STATUSBAR_HEIGHT, SWP_NOZORDER);
    if (hPaletteBar) SetWindowPos(hPaletteBar, NULL, 0, H - PALETTE_HEIGHT - STATUSBAR_HEIGHT, W, PALETTE_HEIGHT, SWP_NOZORDER);
    if (hStatusBar)  SetWindowPos(hStatusBar,  NULL, 0, H - STATUSBAR_HEIGHT, W, STATUSBAR_HEIGHT, SWP_NOZORDER);
}

// ============================================================
//  Message loop
// ============================================================
int Window::runMessageLoop() {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

// ============================================================
//  Public helpers
// ============================================================
void Window::clear() {
    if (!hCanvas) return;
    HDC hdc = GetDC(hCanvas);
    RECT r;
    GetClientRect(hCanvas, &r);
    HBRUSH br = CreateSolidBrush(backgroundColorRef());
    FillRect(hdc, &r, br);
    DeleteObject(br);
    ReleaseDC(hCanvas, hdc);
}

void Window::refresh() {
    if (hCanvas) InvalidateRect(hCanvas, NULL, TRUE);
}

void Window::repaintAll() {
    if (handle)     InvalidateRect(handle,     NULL, TRUE);
    if (hToolbar)   InvalidateRect(hToolbar,   NULL, TRUE);
    if (hSidebar)   InvalidateRect(hSidebar,   NULL, TRUE);
    if (hPaletteBar)InvalidateRect(hPaletteBar,NULL, TRUE);
    if (hStatusBar) InvalidateRect(hStatusBar, NULL, TRUE);
    if (hCanvas)    InvalidateRect(hCanvas,    NULL, TRUE);
}

void Window::setBackgroundColor(const Color& color) {
    backgroundColor = color;
}

void Window::setCursor(HCURSOR hc) {
    currentCursor = hc;
    SetCursor(hc);
}

void Window::setTheme(ThemeMode t) {
    currentTheme = t;
    repaintAll();
}

void Window::toggleTheme() {
    currentTheme = (currentTheme == ThemeMode::Dark) ? ThemeMode::Light : ThemeMode::Dark;
    repaintAll();
}

void Window::setStatusText(const std::wstring& text) {
    statusText = text;
    if (hStatusBar) InvalidateRect(hStatusBar, NULL, TRUE);
}

void Window::setModeText(const std::wstring& text) {
    modeText = text;
    if (hPaletteBar) InvalidateRect(hPaletteBar, NULL, TRUE);
}

void Window::setPaintCallback(PaintCallback callback, void* context) {
    paintCallback = callback;
    paintContext  = context;
}

void Window::setMouseClickCallback(MouseCallback callback, void* context) {
    mouseClickCallback = callback;
    mouseClickContext  = context;
}

void Window::setMouseMoveCallback(MouseCallback callback, void* context) {
    mouseMoveCallback = callback;
    mouseMoveContext  = context;
}

void Window::setRightClickCallback(RightClickCallback callback, void* context) {
    rightClickCallback = callback;
    rightClickContext  = context;
}

void Window::setCommandCallback(CommandCallback callback, void* context) {
    commandCallback = callback;
    commandContext  = context;
}

HWND Window::getHandle() const { return handle; }

RECT Window::getCanvasRect() const {
    RECT r = {0,0,0,0};
    if (hCanvas) GetClientRect(hCanvas, &r);
    return r;
}

Point Window::screenToCanvas(int screenX, int screenY) const {
    if (!hCanvas) return Point(screenX, screenY);
    POINT p = { screenX, screenY };
    ScreenToClient(hCanvas, &p);
    return Point(p.x, p.y);
}

void Window::setSidebarActiveItem(int idm) {
    activeSidebarItem = idm;
    if (hSidebar) InvalidateRect(hSidebar, NULL, TRUE);
}

COLORREF Window::backgroundColorRef() const {
    return RGB(backgroundColor.r, backgroundColor.g, backgroundColor.b);
}

// ============================================================
//  Drawing helpers
// ============================================================
void Window::drawRoundRect(HDC hdc, RECT r, int rx, COLORREF fill, COLORREF border) {
    HBRUSH br = CreateSolidBrush(fill);
    HPEN   pn = CreatePen(PS_SOLID, 1, border);
    HBRUSH old_br = (HBRUSH)SelectObject(hdc, br);
    HPEN   old_pn = (HPEN)SelectObject(hdc, pn);
    RoundRect(hdc, r.left, r.top, r.right, r.bottom, rx, rx);
    SelectObject(hdc, old_br);
    SelectObject(hdc, old_pn);
    DeleteObject(br);
    DeleteObject(pn);
}

void Window::drawText(HDC hdc, const wchar_t* text, RECT r, COLORREF color, int size, bool bold, DWORD flags) {
    HFONT fnt = bold ? fontBold : fontNormal;
    HFONT old = (HFONT)SelectObject(hdc, fnt);
    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);
    DrawTextW(hdc, text, -1, &r, flags);
    SelectObject(hdc, old);
}

// ============================================================
//  paintToolbar
// ============================================================
void Window::paintToolbar(HDC hdc, HWND hwnd) {
    ThemeColors tc = themeFor(currentTheme);
    RECT rc;
    GetClientRect(hwnd, &rc);

    // Background
    HBRUSH bg = CreateSolidBrush(tc.toolbar);
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    // Bottom border
    HPEN pen = CreatePen(PS_SOLID, 1, tc.border);
    HPEN old = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, 0, rc.bottom-1, NULL);
    LineTo(hdc, rc.right, rc.bottom-1);
    SelectObject(hdc, old);
    DeleteObject(pen);

    // App title (left side)
    RECT titleR = {8, 0, 130, rc.bottom};
    HFONT bigFont = CreateFont(-14, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    HFONT oldF = (HFONT)SelectObject(hdc, bigFont);
    SetTextColor(hdc, tc.accent);
    SetBkMode(hdc, TRANSPARENT);
    DrawTextW(hdc, L"Drawing App", -1, &titleR, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oldF);
    DeleteObject(bigFont);

    // Separator after title
    int x = 130;
    HPEN sep = CreatePen(PS_SOLID, 1, tc.border);
    old = (HPEN)SelectObject(hdc, sep);
    MoveToEx(hdc, x, 8, NULL); LineTo(hdc, x, rc.bottom-8);
    SelectObject(hdc, old);
    DeleteObject(sep);
    x += 8;

    // Toolbar buttons
    const int BTN_W = 52;
    const int BTN_H = 34;
    const int BTN_Y = (TOOLBAR_HEIGHT - BTN_H) / 2;

    for (auto& btn : toolbarButtons) {
        if (btn.idm == -2) { // separator
            btn.rect = {x, 0, x+8, TOOLBAR_HEIGHT};
            HPEN sp = CreatePen(PS_SOLID, 1, tc.border);
            HPEN op = (HPEN)SelectObject(hdc, sp);
            MoveToEx(hdc, x+3, 10, NULL); LineTo(hdc, x+3, rc.bottom-10);
            SelectObject(hdc, op);
            DeleteObject(sp);
            x += 10;
            continue;
        }
        if (btn.idm == -3) { // Theme toggle — right-aligned, skip in normal loop
            continue;
        }

        RECT btnR = {x, BTN_Y, x + BTN_W, BTN_Y + BTN_H};
        btn.rect = btnR;

        bool isActive = (btn.idm == activeSidebarItem);
        COLORREF btnBg = isActive ? tc.itemActive : tc.toolbar;
        COLORREF btnBorder = isActive ? tc.accent : tc.border;
        COLORREF txtColor = isActive ? tc.accent : tc.textMuted;
        drawRoundRect(hdc, btnR, 5, btnBg, btnBorder);
        drawText(hdc, btn.label.c_str(), btnR, txtColor, 11, isActive, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        x += BTN_W + 3;
    }

    // Theme toggle button — right-aligned
    const int THEME_W = 80;
    RECT themeR = {rc.right - THEME_W - 8, BTN_Y, rc.right - 8, BTN_Y + BTN_H};
    COLORREF themeBg = (currentTheme == ThemeMode::Dark) ? RGB(40,40,70) : RGB(220,230,250);
    COLORREF themeBorder = tc.accent;
    drawRoundRect(hdc, themeR, 14, themeBg, themeBorder);
    const wchar_t* themeLabel = (currentTheme == ThemeMode::Dark) ? L"☀ Light" : L"☾ Dark";
    drawText(hdc, themeLabel, themeR, tc.accent, 11, false, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    // Store in last toolbar button
    for (auto& btn : toolbarButtons) {
        if (btn.idm == -3) { btn.rect = themeR; break; }
    }
}

// ============================================================
//  paintSidebar
// ============================================================
void Window::paintSidebar(HDC hdc, HWND hwnd) {
    ThemeColors tc = themeFor(currentTheme);
    RECT rc;
    GetClientRect(hwnd, &rc);

    HBRUSH bg = CreateSolidBrush(tc.sidebar);
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    // Right border
    HPEN pen = CreatePen(PS_SOLID, 1, tc.border);
    HPEN old = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, rc.right-1, 0, NULL);
    LineTo(hdc, rc.right-1, rc.bottom);
    SelectObject(hdc, old);
    DeleteObject(pen);

    const int ITEM_H    = 28;
    const int SECTION_H = 24;

    // --- Compute total content height for scrollbar ---
    int totalH = 4;
    for (const auto& item : sidebarItems)
        totalH += item.isSection ? SECTION_H : ITEM_H;
    totalH += 4; // bottom padding

    // --- Update vertical scrollbar ---
    SCROLLINFO si = {};
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin   = 0;
    si.nMax   = totalH - 1;
    si.nPage  = (UINT)rc.bottom;
    si.nPos   = sidebarScrollY;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
    // Clamp scroll position after SetScrollInfo (it may have adjusted nPos)
    GetScrollInfo(hwnd, SB_VERT, &si);
    sidebarScrollY = si.nPos;

    int y = 4 - sidebarScrollY; // shift items up by scroll offset

    HFONT oldFont = (HFONT)SelectObject(hdc, fontNormal);

    for (auto& item : sidebarItems) {
        if (item.isSection) {
            item.rect = {0, y, rc.right-1, y + SECTION_H};
            if (y + SECTION_H > 0 && y < rc.bottom) {
                RECT labelR = {10, y+6, rc.right-4, y + SECTION_H};
                HFONT f = (HFONT)SelectObject(hdc, fontSection);
                SetTextColor(hdc, tc.sectionLabel);
                SetBkMode(hdc, TRANSPARENT);
                DrawTextW(hdc, item.label.c_str(), -1, &labelR, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                SelectObject(hdc, f);
            }
            y += SECTION_H;
        } else {
            item.rect = {0, y, rc.right-1, y + ITEM_H};
            if (y + ITEM_H > 0 && y < rc.bottom) {
                bool isActive = (item.idm == activeSidebarItem);

                if (isActive) {
                    HBRUSH ab = CreateSolidBrush(tc.itemActive);
                    FillRect(hdc, &item.rect, ab);
                    DeleteObject(ab);
                    HPEN ap = CreatePen(PS_SOLID, 3, tc.accent);
                    HPEN op2 = (HPEN)SelectObject(hdc, ap);
                    MoveToEx(hdc, 0, y+2, NULL);
                    LineTo(hdc, 0, y + ITEM_H - 2);
                    SelectObject(hdc, op2);
                    DeleteObject(ap);
                }

                COLORREF txtColor = isActive ? tc.accent : tc.text;
                HFONT f = (HFONT)SelectObject(hdc, isActive ? fontBold : fontNormal);
                RECT textR = {14, y, rc.right-4, y + ITEM_H};
                SetTextColor(hdc, txtColor);
                SetBkMode(hdc, TRANSPARENT);
                DrawTextW(hdc, item.label.c_str(), -1, &textR, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                SelectObject(hdc, f);
            }
            y += ITEM_H;
        }
    }

    SelectObject(hdc, oldFont);
}

// ============================================================
//  paintPaletteBar
// ============================================================
void Window::paintPaletteBar(HDC hdc, HWND hwnd) {
    ThemeColors tc = themeFor(currentTheme);
    RECT rc;
    GetClientRect(hwnd, &rc);

    HBRUSH bg = CreateSolidBrush(tc.palette);
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    // Top border
    HPEN pen = CreatePen(PS_SOLID, 1, tc.border);
    HPEN old = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, 0, 0, NULL);
    LineTo(hdc, rc.right, 0);
    SelectObject(hdc, old);
    DeleteObject(pen);

    // "COLOR" label
    RECT lbl = {10, 0, 60, rc.bottom};
    drawText(hdc, L"COLOR", lbl, tc.textMuted, 10, false, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Active colour large swatch (shows current drawing colour)
    int ax = 60;
    RECT activeSw = {ax, (rc.bottom - 28)/2, ax+28, (rc.bottom - 28)/2 + 28};
    HBRUSH aBr = CreateSolidBrush(PALETTE_COLORS[activePaletteIndex]);
    HPEN   aPn = CreatePen(PS_SOLID, 2, tc.accent);
    HBRUSH oa = (HBRUSH)SelectObject(hdc, aBr);
    HPEN   op = (HPEN)SelectObject(hdc, aPn);
    RoundRect(hdc, activeSw.left, activeSw.top, activeSw.right, activeSw.bottom, 5, 5);
    SelectObject(hdc, oa); SelectObject(hdc, op);
    DeleteObject(aBr); DeleteObject(aPn);

    // Separator
    int sx = ax + 36;
    HPEN sp = CreatePen(PS_SOLID, 1, tc.border);
    HPEN os = (HPEN)SelectObject(hdc, sp);
    MoveToEx(hdc, sx, 8, NULL); LineTo(hdc, sx, rc.bottom-8);
    SelectObject(hdc, os); DeleteObject(sp);

    // Palette label
    RECT pl = {sx+6, 0, sx+60, rc.bottom};
    drawText(hdc, L"PALETTE", pl, tc.textMuted, 10, false, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Swatches
    int swx = sx + 60;
    int swy = (rc.bottom - SWATCH_SIZE) / 2;
    for (int i = 0; i < PALETTE_COUNT; i++) {
        RECT sw = {swx, swy, swx + SWATCH_SIZE, swy + SWATCH_SIZE};
        bool selected = (i == activePaletteIndex);
        HBRUSH sbr = CreateSolidBrush(PALETTE_COLORS[i]);
        HPEN   spn = CreatePen(PS_SOLID, selected ? 2 : 1,
                               selected ? tc.accent : tc.border);
        HBRUSH os2 = (HBRUSH)SelectObject(hdc, sbr);
        HPEN   op2 = (HPEN)SelectObject(hdc, spn);
        Ellipse(hdc, sw.left, sw.top, sw.right, sw.bottom);
        SelectObject(hdc, os2); SelectObject(hdc, op2);
        DeleteObject(sbr); DeleteObject(spn);
        swx += SWATCH_SIZE + SWATCH_GAP;
    }

    // Mode label (right side)
    RECT modeR = {rc.right - 320, 0, rc.right - 10, rc.bottom};
    drawText(hdc, modeText.c_str(), modeR, tc.accent, 12, true, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
}

// ============================================================
//  paintStatusBar
// ============================================================
void Window::paintStatusBar(HDC hdc, HWND hwnd) {
    ThemeColors tc = themeFor(currentTheme);
    RECT rc;
    GetClientRect(hwnd, &rc);

    HBRUSH bg = CreateSolidBrush(tc.statusBar);
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    // Top border
    HPEN pen = CreatePen(PS_SOLID, 1, tc.border);
    HPEN old = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, 0, 0, NULL); LineTo(hdc, rc.right, 0);
    SelectObject(hdc, old); DeleteObject(pen);

    // Status dot
    HBRUSH dot = CreateSolidBrush(tc.accent);
    HBRUSH od = (HBRUSH)SelectObject(hdc, dot);
    HPEN np = CreatePen(PS_NULL, 0, 0);
    HPEN op = (HPEN)SelectObject(hdc, np);
    Ellipse(hdc, 10, rc.bottom/2 - 4, 18, rc.bottom/2 + 4);
    SelectObject(hdc, od); SelectObject(hdc, op);
    DeleteObject(dot); DeleteObject(np);

    RECT textR = {24, 0, rc.right/2, rc.bottom};
    drawText(hdc, statusText.c_str(), textR, tc.textMuted, 10, false, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

// ============================================================
//  Hit testing
// ============================================================
int Window::hitTestSidebar(int y) const {
    for (const auto& item : sidebarItems) {
        if (!item.isSection && y >= item.rect.top && y < item.rect.bottom)
            return item.idm;
    }
    return -1;
}

// ============================================================
//  Static WndProc — main window
// ============================================================
LRESULT CALLBACK Window::windowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Window* self = nullptr;
    if (msg == WM_NCCREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lp);
        self = reinterpret_cast<Window*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->handle = hwnd;
    } else {
        self = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    if (self) return self->handleMessage(msg, wp, lp);
    return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT Window::handleMessage(UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_SIZE:
        layoutChildren();
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(handle, &ps);
        ThemeColors tc = themeFor(currentTheme);
        HBRUSH br = CreateSolidBrush(tc.bg);
        RECT rc; GetClientRect(handle, &rc);
        FillRect(hdc, &rc, br);
        DeleteObject(br);
        EndPaint(handle, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1; // prevent flicker

    case WM_SETCURSOR:
        if (LOWORD(lp) == HTCLIENT && currentCursor) {
            SetCursor(currentCursor);
            return TRUE;
        }
        return DefWindowProc(handle, msg, wp, lp);

    case WM_DESTROY:
        destroyFonts();
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(handle, msg, wp, lp);
    }
}

// ============================================================
//  Static WndProc — sidebar
// ============================================================
LRESULT CALLBACK Window::sidebarProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Window* self = reinterpret_cast<Window*>(GetWindowLongPtr(GetParent(hwnd), GWLP_USERDATA));
    if (self) return self->handleSidebar(hwnd, msg, wp, lp);
    return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT Window::handleSidebar(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        paintSidebar(hdc, hwnd);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        int y = GET_Y_LPARAM(lp);
        int idm = hitTestSidebar(y);   // hitTestSidebar uses item.rect which already includes scroll offset
        if (idm > 0) {
            setSidebarActiveItem(idm);
            InvalidateRect(hToolbar, NULL, TRUE);
            if (commandCallback) commandCallback(idm, commandContext);
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_VSCROLL: {
        SCROLLINFO si = {};
        si.cbSize = sizeof(si);
        si.fMask  = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        int newPos = si.nPos;
        switch (LOWORD(wp)) {
            case SB_LINEUP:        newPos -= 28;          break;
            case SB_LINEDOWN:      newPos += 28;          break;
            case SB_PAGEUP:        newPos -= si.nPage;    break;
            case SB_PAGEDOWN:      newPos += si.nPage;    break;
            case SB_THUMBTRACK:
            case SB_THUMBPOSITION: newPos  = si.nTrackPos; break;
            case SB_TOP:           newPos  = si.nMin;      break;
            case SB_BOTTOM:        newPos  = si.nMax;      break;
        }
        // Clamp
        int maxPos = si.nMax - (int)si.nPage + 1;
        if (maxPos < 0) maxPos = 0;
        if (newPos < 0)      newPos = 0;
        if (newPos > maxPos) newPos = maxPos;

        if (newPos != sidebarScrollY) {
            sidebarScrollY = newPos;
            si.fMask = SIF_POS;
            si.nPos  = sidebarScrollY;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    }

    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wp);
        // Positive delta = scroll up, negative = scroll down
        int lines = delta / WHEEL_DELTA * 3;  // 3 items per notch
        SCROLLINFO si = {};
        si.cbSize = sizeof(si);
        si.fMask  = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        int newPos = sidebarScrollY - lines * 28;
        int maxPos = si.nMax - (int)si.nPage + 1;
        if (maxPos < 0) maxPos = 0;
        if (newPos < 0)      newPos = 0;
        if (newPos > maxPos) newPos = maxPos;

        if (newPos != sidebarScrollY) {
            sidebarScrollY = newPos;
            si.fMask = SIF_POS;
            si.nPos  = sidebarScrollY;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    }

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
}

// ============================================================
//  Static WndProc — toolbar
// ============================================================
LRESULT CALLBACK Window::toolbarProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Window* self = reinterpret_cast<Window*>(GetWindowLongPtr(GetParent(hwnd), GWLP_USERDATA));
    if (self) return self->handleToolbar(hwnd, msg, wp, lp);
    return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT Window::handleToolbar(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        paintToolbar(hdc, hwnd);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        int x = GET_X_LPARAM(lp);
        int y = GET_Y_LPARAM(lp);
        for (auto& btn : toolbarButtons) {
            if (x >= btn.rect.left && x < btn.rect.right &&
                y >= btn.rect.top  && y < btn.rect.bottom) {
                if (btn.idm == -3) {
                    // Theme toggle
                    toggleTheme();
                } else if (btn.idm > 0) {
                    setSidebarActiveItem(btn.idm);
                    if (commandCallback) commandCallback(btn.idm, commandContext);
                }
                break;
            }
        }
        return 0;
    }

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
}

// ============================================================
//  Static WndProc — palette bar
// ============================================================
LRESULT CALLBACK Window::paletteProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Window* self = reinterpret_cast<Window*>(GetWindowLongPtr(GetParent(hwnd), GWLP_USERDATA));
    if (self) return self->handlePaletteBar(hwnd, msg, wp, lp);
    return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT Window::handlePaletteBar(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        paintPaletteBar(hdc, hwnd);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        int x = GET_X_LPARAM(lp);
        int y = GET_Y_LPARAM(lp);
        RECT rc; GetClientRect(hwnd, &rc);

        // Swatch hit test — must mirror paintPaletteBar layout
        int sx  = 60 + 36 + 60; // after active-swatch + sep + "PALETTE" label
        int swy = (rc.bottom - SWATCH_SIZE) / 2;
        for (int i = 0; i < PALETTE_COUNT; i++) {
            int cx = sx + i*(SWATCH_SIZE + SWATCH_GAP) + SWATCH_SIZE/2;
            int cy = swy + SWATCH_SIZE/2;
            int dx = x - cx, dy = y - cy;
            if (dx*dx + dy*dy <= (SWATCH_SIZE/2)*(SWATCH_SIZE/2)) {
                activePaletteIndex = i;
                InvalidateRect(hwnd, NULL, TRUE);
                // Notify application via commandCallback with a special ID
                // We encode palette selection as IDM_PREF_COLOR range offset
                // Application checks and calls setDrawingColor with the right COLORREF
                // Simplest: just fire IDM_PREF_COLOR with the colour pre-set
                if (commandCallback) {
                    // We send a custom message directly to main window
                    PostMessage(handle, WM_APP + 1, (WPARAM)PALETTE_COLORS[i], 0);
                }
                break;
            }
        }
        return 0;
    }

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
}

// ============================================================
//  Static WndProc — status bar
// ============================================================
LRESULT CALLBACK Window::statusProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Window* self = reinterpret_cast<Window*>(GetWindowLongPtr(GetParent(hwnd), GWLP_USERDATA));
    if (self) return self->handleStatusBar(hwnd, msg, wp, lp);
    return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT Window::handleStatusBar(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        paintStatusBar(hdc, hwnd);
        EndPaint(hwnd, &ps);
        return 0;
    }
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
}

// In Window.cpp — add this static proc:
LRESULT CALLBACK Window::canvasProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Window* self = reinterpret_cast<Window*>(
        GetWindowLongPtr(GetParent(hwnd), GWLP_USERDATA));
    if (!self) return DefWindowProc(hwnd, msg, wp, lp);

    switch (msg) {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            // Fill white background
            RECT rc; GetClientRect(hwnd, &rc);
            HBRUSH bg = CreateSolidBrush(self->backgroundColorRef());
            FillRect(hdc, &rc, bg);
            DeleteObject(bg);
            // Fire the paint callback so Application::render() draws shapes
            if (self->paintCallback)
                self->paintCallback(hdc, self->paintContext);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            Point p = self->screenToCanvas(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            // screenToCanvas uses ScreenToClient(hCanvas,...) but lp coords are
            // already client-relative to the canvas, so just use them directly:
            p = Point(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (self->mouseClickCallback)
                self->mouseClickCallback(p, self->mouseClickContext);
            return 0;
        }

        case WM_MOUSEMOVE: {
            Point p(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (self->mouseMoveCallback)
                self->mouseMoveCallback(p, self->mouseMoveContext);
            return 0;
        }

        case WM_RBUTTONDOWN: {
            Point p(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (self->rightClickCallback)
                self->rightClickCallback(p, self->rightClickContext);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
}