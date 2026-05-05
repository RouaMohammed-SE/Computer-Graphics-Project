#ifndef MENU_MENU_H
#define MENU_MENU_H

#include "../Core/Enums.h"

// Stores and updates current drawing mode and selected algorithms.
class Menu {
public:
    Menu();

    void setMode(DrawingMode mode);
    DrawingMode getMode() const;

    void setLineAlgorithm(LineAlgorithmType algorithm);
    void setCircleAlgorithm(CircleAlgorithmType algorithm);
    void setEllipseAlgorithm(EllipseAlgorithmType algorithm);

private:
    DrawingMode currentMode;
    LineAlgorithmType lineAlgorithm;
    CircleAlgorithmType circleAlgorithm;
    EllipseAlgorithmType ellipseAlgorithm;
};

#endif
