#include "InputHandler.h"

InputHandler::InputHandler() : lastMousePosition() {
}

void InputHandler::processMouseClick(const Point& position) {
    lastMousePosition = position;
}

void InputHandler::processMouseMove(const Point& position) {
    lastMousePosition = position;
}

Point InputHandler::getLastMousePosition() const {
    return lastMousePosition;
}
