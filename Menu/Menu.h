#pragma once
#ifndef MENU_MENU_H
#define MENU_MENU_H

#include "../Core/Enums.h"

// Stores and updates current drawing mode and selected algorithms.
class Menu {
private:
    DrawingMode currentMode;
    LineAlgorithmType lineAlgorithm;
    CircleAlgorithmType circleAlgorithm;
    EllipseAlgorithmType ellipseAlgorithm;
    FillAlgorithmType fillAlgorithm;
    ClippingType clippingType;
    ClipAlgorithmType clipAlgorithm;

public:
    Menu();

    void setMode(DrawingMode mode);
    DrawingMode getMode() const;

    void setLineAlgorithm(LineAlgorithmType algorithm);
    LineAlgorithmType getLineAlgorithm() const;

    void setCircleAlgorithm(CircleAlgorithmType algorithm);
    CircleAlgorithmType getCircleAlgorithm() const;

    void setEllipseAlgorithm(EllipseAlgorithmType algorithm);
    EllipseAlgorithmType getEllipseAlgorithm() const;

    void setFillAlgorithm(FillAlgorithmType algorithm);
    FillAlgorithmType getFillAlgorithm() const;

    void setClippingType(ClippingType type);
    ClippingType getClippingType() const;
    void setClipAlgorithm(ClipAlgorithmType algo);
    ClipAlgorithmType getClipAlgorithm() const;

};
#endif
