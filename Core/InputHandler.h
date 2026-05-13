#pragma once
#ifndef CORE_INPUT_HANDLER_H
#define CORE_INPUT_HANDLER_H

#include "../Utils/Point.h"

// Handles mouse-only input and converts events into drawing points.
class InputHandler {
public:
    InputHandler();

    void processMouseClick(const Point& position);
    void processMouseMove(const Point& position);
    Point getLastMousePosition() const;

private:
    Point lastMousePosition;
};

#endif
