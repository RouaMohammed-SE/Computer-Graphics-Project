# Computer Graphics Project

A 2D drawing application built in C++ using the Windows GDI API. The project demonstrates core computer graphics algorithms for drawing, filling, and clipping shapes — implemented from scratch without relying on any graphics library.

---

## Features

### Drawing Algorithms

**Lines**
- DDA (Digital Differential Analyzer)
- Midpoint Line
- Parametric Line

**Circles**
- Direct (Cartisean)
- Polar
- Iterative Polar
- Midpoint
- Modified Midpoint

**Ellipses**
- Direct (Cartisean)
- Polar
- Iterative Polar
- Midpoint

**Curves**
- Cardinal Spline

### Filling Algorithms
- Flood Fill (Recursive)
- Flood Fill (Non-Recursive)
- Circle Fill with Lines (by quarter)
- Circle Fill with Circles (gradient, inner/outer radius)
- Rectangle Fill with horizontal Bezier Curves
- Square Fill with Vertical Hermite Curves
- Convex Polygon Fill
- Non-Convex Polygon Fill

### Clipping Algorithms
- Rectangle: Point, Line, and Polygon clipping
- Square: Point and Line clipping
- Circle: Point and Line clipping

### Other
- Smiley Face drawing (happy / sad)
- Drawing color picker
- Background color toggle
- Cursor style selection (Default, Crosshair, Hand)
- Save / Load canvas state
- Clear canvas

---

## Requirements

- **OS:** Windows (uses Win32 API and GDI)
- **Compiler:** C++17 or later
- **Build System:** CMake 3.20+
- **IDE:** CLion or Visual Studio (Both configs are included)

---

## Build & Run

```bash
# Clone the repository
git clone <repo-url>
cd Computer-Graphics-Project

# Configure and build with CMake
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug

# Run the executable
./cmake-build-debug/ComputerGraphics.exe
```

Or open the project in **CLion** and click **Run** — the CMake configuration is already set up.

---

## Usage

All features are accessible from the **menu bar** at the top of the window:

1. **Select a tool** from the menu (e.g., Line → DDA).
2. **Click on the canvas** to define points (start/end for lines, center/radius for circles, etc.).
3. For clipping and filling operations, follow the on-screen flow: define the clipping region first, then the shape or point.
4. Use **File → Save / Load** to persist your drawing.
5. Use **Preferences** to change drawing color, background color, or cursor style.
