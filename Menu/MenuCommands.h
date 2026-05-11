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

#endif