#pragma once
#ifndef MENU_MENU_COMMANDS_H
#define MENU_MENU_COMMANDS_H

// These IDs are sent to WM_COMMAND whenever the user clicks a menu item.
// WinAPI requires each menu item to have a unique integer ID.
#define IDM_FILE_CLEAR           1001
#define IDM_FILE_SAVE            1002
#define IDM_FILE_LOAD            1003
#define IDM_PREF_BG_WHITE        1004
#define IDM_PREF_CURSOR_DEFAULT  1005
#define IDM_PREF_CURSOR_CROSSHAIR 1006
#define IDM_PREF_CURSOR_HAND     1007
#define IDM_PREF_COLOR           1008

// Line Menu
#define IDM_PARAMETRIC_LINE      1101
#define IDM_DDA_LINE             1102
#define IDM_MIDPOINT_LINE        1103

// Circle Menu
#define IDM_DIRECT_CIRCLE               1104
#define IDM_POLAR_CIRCLE                1105
#define IDM_ITERATIVE_POLAR_CIRCLE      1106
#define IDM_MIDPOINT_CIRCLE             1107
#define IDM_MODIFIED_MIDPOINT_CIRCLE    1108

// Ellipse Menu
#define IDM_DIRECT_ELLIPSE              1109
#define IDM_POLAR_ELLIPSE               1110
#define IDM_ITERATIVE_POLAR_ELLIPSE     1111
#define IDM_MIDPOINT_ELLIPSE            1112

// Curves Menu
#define IDM_CARDINAL_SPLINE_CURVE      1113

// Filling Menu
#define IDM_FILL_CIRCLE_WITH_LINES    1114
#define IDM_FILL_CIRCLE_WITH_CIRCLES  1115
#define IDM_FILL_RECTANGLE_CURVES     1116
#define IDM_FILL_SQUARE_CURVES        1117
#define IDM_FILL_CONVEX               1118
#define IDM_FILL_NON_CONVEX           1119
#define IDM_FILL_FLOOD_RECURSIVE      1120
#define IDM_FILL_FLOOD_NON_RECURSIVE   1121

// Clipping Menu
#define IDM_CLIPPING_RECTANGLE_POINT    1122
#define IDM_CLIPPING_RECTANGLE_LINE     1123
#define IDM_CLIPPING_RECTANGLE_POLYGON  1124
#define IDM_CLIPPING_SQUARE_POINT       1125
#define IDM_CLIPPING_SQUARE_LINE        1126
#define IDM_CLIPPING_CIRCLE_POINT       1127
#define IDM_CLIPPING_CIRCLE_LINE        1128

#define IDM_SMILEY_FACE_HAPPY           1129
#define IDM_SMILEY_FACE_SAD             1130
#endif
