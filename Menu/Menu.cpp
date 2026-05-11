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

void Menu::setLineAlgorithm(LineAlgorithmType algorithm) {
    lineAlgorithm = algorithm;
}

void Menu::setCircleAlgorithm(CircleAlgorithmType algorithm) {
    circleAlgorithm = algorithm;
}

void Menu::setEllipseAlgorithm(EllipseAlgorithmType algorithm) {
    ellipseAlgorithm = algorithm;
}
