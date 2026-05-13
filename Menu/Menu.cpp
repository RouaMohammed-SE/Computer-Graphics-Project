#include "Menu.h"

Menu::Menu()
    : currentMode(DrawingMode::None),
      lineAlgorithm(LineAlgorithmType::DDA),
      circleAlgorithm(CircleAlgorithmType::Midpoint),
      ellipseAlgorithm(EllipseAlgorithmType::Midpoint) {
    // TODO: Add initialization logic if needed.
}

void Menu::setMode(DrawingMode mode) {
    currentMode = mode;
}

DrawingMode Menu::getMode() const {
    return currentMode;
}

void Menu::setLineAlgorithm(LineAlgorithmType a) {
    lineAlgorithm = a;
}

LineAlgorithmType Menu::getLineAlgorithm() const {
    return lineAlgorithm;
}

void Menu::setCircleAlgorithm(CircleAlgorithmType algorithm) {
    circleAlgorithm = algorithm;
}

CircleAlgorithmType Menu::getCircleAlgorithm() const {
    return circleAlgorithm;
}

void Menu::setEllipseAlgorithm(EllipseAlgorithmType algorithm) {
    ellipseAlgorithm = algorithm;
}

EllipseAlgorithmType Menu::getEllipseAlgorithm() const {
    return ellipseAlgorithm;
}

void Menu::setFillAlgorithm(FillAlgorithmType a) {
    fillAlgorithm = a;
}

FillAlgorithmType Menu::getFillAlgorithm() const {
    return fillAlgorithm;
}

void Menu::setClippingType(ClippingType t) {
    clippingType = t;
}

ClippingType Menu::getClippingType() const {
    return clippingType;
}

void Menu::setClipAlgorithm(ClipAlgorithmType a) {
    clipAlgorithm = a;
}

ClipAlgorithmType Menu::getClipAlgorithm() const {
    return clipAlgorithm;
}

