#include "Menu.h"

Menu::Menu()
    : currentMode(DrawingMode::None),
      lineAlgorithm(LineAlgorithmType::DDA),
      circleAlgorithm(CircleAlgorithmType::Midpoint),
      ellipseAlgorithm(EllipseAlgorithmType::Midpoint) {
    // TODO: Add initialization logic if needed.
}

void Menu::setMode(DrawingMode mode) {
    // TODO: Store selected mode.
}

DrawingMode Menu::getMode() const {
    // TODO: Return selected mode.
    return DrawingMode::None;
}

void Menu::setLineAlgorithm(LineAlgorithmType algorithm) {
    // TODO: Store selected line algorithm.
}

void Menu::setCircleAlgorithm(CircleAlgorithmType algorithm) {
    // TODO: Store selected circle algorithm.
}

void Menu::setEllipseAlgorithm(EllipseAlgorithmType algorithm) {
    // TODO: Store selected ellipse algorithm.
}
