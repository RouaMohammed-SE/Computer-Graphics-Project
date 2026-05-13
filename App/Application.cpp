#include "Application.h"
#include "../Menu/MenuCommands.h"
#include "../Filling/FillAlgorithms.h"
#include <commdlg.h>
#include <iostream>


Application::Application()
    : window(800, 600, Color(255, 255, 255)),
      inputHandler(),
      menu(),
      fileManager(),
      preferences(),
      logger(),
      shapes(),
      drawingColor(0, 0, 0),
      backgroundColor(255, 255, 255) {
    window.setPaintCallback(Application::handlePaint, this);
    window.setMouseClickCallback(Application::handleMouseClick, this);
    window.setMouseMoveCallback(Application::handleMouseMove, this);
    window.setCommandCallback(Application::handleCommand, this);
}

Application::~Application() {
    // Release owned shapes.
    for (Shape* shape : shapes) {
        delete shape;
    }
    shapes.clear();
}

void Application::run() {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$",  "r", stdin);

    logger.log("Application started.");
    logger.log("Use the menus at the top of the window to draw shapes.");

    if (!window.create(L"Drawing Package")) {
        logger.error("Failed to create window.");
        return;
    }

    window.runMessageLoop();
}

void Application::update() {
    // TODO: Process input and menu state.
}

void Application::render(HDC hdc) {
    // Draw all stored shapes using the provided WinAPI HDC.
    for (Shape* shape : shapes) {
        shape->draw(hdc);
    }
}

void Application::addShape(Shape* shape) {
    if (shape) shapes.push_back(shape);
}

void Application::clearShapes() {
    // Release owned shapes.
    for (Shape* shape : shapes) {
        delete shape;
    }
    shapes.clear();
    logger.log("Cleared all shapes.");
}

void Application::setDrawingColor(const Color& color) {
    drawingColor = color;
    preferences.setDrawingColor(color);
}

void Application::setBackgroundColor(const Color& color) {
    backgroundColor = color;
    preferences.setBackgroundColor(color);
    window.setBackgroundColor(color);
}

void Application::handlePaint(HDC hdc, void* context) {
    Application* app = static_cast<Application*>(context);
    app->render(hdc);
}

void Application::handleMouseClick(const Point& position, void* context) {
    Application* app = static_cast<Application*>(context);
    app->inputHandler.processMouseClick(position);
    app->update();
}

void Application::handleMouseMove(const Point& position, void* context) {
    Application* app = static_cast<Application*>(context);
    app->inputHandler.processMouseMove(position);
}

// handleCommand
// Dispatches every menu click (WM_COMMAND) to the right action.

void Application::handleCommand(int commandId, void* context) {
    Application* app = static_cast<Application*>(context);

    switch (commandId) {

        //File Menu commands
        case IDM_FILE_CLEAR:
            // 1a: clear all shapes from the screen
            app->clearShapes();
            app->window.clear();
            app->window.refresh();
            app->logger.log("Screen cleared.");
            break;

        case IDM_FILE_SAVE: {
            //1b: save all drawn shapes to a file
            // Requirement 10: take file path from the console
            std::string path;
            std::cout << "[INPUT] Enter file path to save (e.g. drawing.cg): ";
            std::cin  >> path;
            app->fileManager.saveShapes(path, app->shapes);
            break;
        }

        case IDM_FILE_LOAD: {
            //1c: load saved shapes from a file
            std::string path;
            std::cout << "[INPUT] Enter file path to load: ";
            std::cin  >> path;
            app->fileManager.loadShapes(path, app->shapes);
            app->window.refresh();   // Repaint so loaded shapes appear
            break;
        }

        // Preferences Menu commands
        case IDM_PREF_BG_WHITE:
            // Assignment 2a: change window background to white
            app->setBackgroundColor(Color(255, 255, 255));
            app->window.clear();
            app->window.refresh();
            app->logger.log("Background set to white.");
            break;

        case IDM_PREF_CURSOR_DEFAULT:
            // 2b: change mouse cursor shape
            app->preferences.setMouseCursor(MouseCursorType::Default);
            app->window.setCursor(LoadCursor(NULL, IDC_ARROW));
            app->logger.log("Cursor: Default.");
            break;

        case IDM_PREF_CURSOR_CROSSHAIR:
            app->preferences.setMouseCursor(MouseCursorType::Crosshair);
            app->window.setCursor(LoadCursor(NULL, IDC_CROSS));
            app->logger.log("Cursor: Crosshair.");
            break;

        case IDM_PREF_CURSOR_HAND:
            app->preferences.setMouseCursor(MouseCursorType::Hand);
            app->window.setCursor(LoadCursor(NULL, IDC_HAND));
            app->logger.log("Cursor: Hand.");
            break;

        case IDM_PREF_COLOR: {
            // 2c: let the user choose the drawing color from a dialog
            static COLORREF customColors[16] = {};
            CHOOSECOLOR cc    = {};
            cc.lStructSize    = sizeof(cc);
            cc.hwndOwner      = app->window.getHandle();
            cc.lpCustColors   = customColors;
            cc.rgbResult      = RGB(app->drawingColor.r, app->drawingColor.g, app->drawingColor.b);
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;

            if (ChooseColor(&cc)) {
                Color chosen(GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));
                app->setDrawingColor(chosen);
                app->logger.log("Drawing color updated via color picker.");
            }
            break;
        }

        default:
            // Handle your own custom menu commands here, or log unhandled ones for debugging.
            break;
    }
}