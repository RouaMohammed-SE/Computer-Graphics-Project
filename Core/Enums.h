#ifndef CORE_ENUMS_H
#define CORE_ENUMS_H

// High-level user interaction modes selected from the menu.
enum class DrawingMode {
    None,
    DrawLine,
    DrawCircle,
    DrawEllipse,
    DrawCurve,
    Fill,
    Clip,
    Save,
    Load,
    Preferences
};

// Supported line drawing algorithms.
enum class LineAlgorithmType {
    DDA,
    Midpoint,
    Parametric
};

// Supported circle drawing algorithms.
enum class CircleAlgorithmType {
    Direct,
    Polar,
    IterativePolar,
    Midpoint,
    ModifiedMidpoint
};

// Supported ellipse drawing algorithms.
enum class EllipseAlgorithmType {
    Direct,
    Polar,
    Midpoint
};

// Supported filling algorithms.
enum class FillAlgorithmType {
    FloodFillRecursive,
    FloodFillNonRecursive,
    CircleFill,
    RectangleFillWithCurves,
    SquareFillWithCurves
};

// Supported clipping targets.
enum class ClippingType {
    Rectangle,
    Square,
    Circle
};

// Mouse cursor styles available through preferences.
enum class MouseCursorType {
    Default,
    Crosshair,
    Hand
};

#endif
