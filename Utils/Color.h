#pragma once
#ifndef UTILS_COLOR_H
#define UTILS_COLOR_H

// Stores RGB color values used for drawing and preferences.
class Color {
public:
    int r;
    int g;
    int b;

    Color();
    Color(int r, int g, int b);
};

#endif
