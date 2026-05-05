#include "Window.h"

Window::Window() : width(0), height(0), backgroundColor() {
    // TODO: Add initialization logic if needed.
}

Window::Window(int width, int height, const Color& backgroundColor)
    : width(width), height(height), backgroundColor(backgroundColor) {
    // TODO: Add initialization logic if needed.
}

void Window::create() {
    // TODO: Create drawing window.
}

void Window::clear() {
    // TODO: Clear drawing surface.
}

void Window::refresh() {
    // TODO: Refresh drawing surface.
}

void Window::setBackgroundColor(const Color& color) {
    // TODO: Set background color.
}
