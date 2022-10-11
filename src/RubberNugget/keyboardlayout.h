#pragma once
#include "Arduino.h"
#include "hidkeyboard.h"

struct duckyKeys {
  String title;
  uint16_t key;
};

// add keycode modifier

duckyKeys keyMapRN[] = {

    {"ENTER", HID_KEY_ENTER},
    {"MENU", HID_KEY_MENU},
    {"DELETE", HID_KEY_DELETE},
    {"BACKSPACE", HID_KEY_BACKSPACE},
    {"HOME", HID_KEY_HOME},
    {"INSERT", HID_KEY_INSERT},
    {"PAGEUP", HID_KEY_PAGE_UP},
    {"PAGEDOWN", HID_KEY_PAGE_DOWN},
    {"UPARROW", HID_KEY_ARROW_UP},
    {"UP", HID_KEY_ARROW_UP},
    {"DOWNARROW", HID_KEY_ARROW_DOWN},
    {"DOWN", HID_KEY_ARROW_DOWN},
    {"LEFTARROW", HID_KEY_ARROW_LEFT},
    {"LEFT", HID_KEY_ARROW_LEFT},
    {"RIGHTARROW", HID_KEY_ARROW_RIGHT},
    {"RIGHT", HID_KEY_ARROW_RIGHT},
    {"TAB", HID_KEY_TAB},
    {"END", HID_KEY_END},
    {"ESC", HID_KEY_ESCAPE},
    {"F1", HID_KEY_F1},
    {"F2", HID_KEY_F2},
    {"F3", HID_KEY_F3},
    {"F4", HID_KEY_F4},
    {"F5", HID_KEY_F5},
    {"F6", HID_KEY_F6},
    {"F7", HID_KEY_F7},
    {"F8", HID_KEY_F8},
    {"F9", HID_KEY_F9},
    {"F10", HID_KEY_F10},
    {"F11", HID_KEY_F11},
    {"F12", HID_KEY_F12},
    {"SPACE", HID_KEY_SPACE},
    {"PAUSE", HID_KEY_PAUSE},
    {"CAPSLOCK", HID_KEY_CAPS_LOCK},
    {"NUMLOCK", HID_KEY_NUM_LOCK},
    {"PRINTSCREEN", HID_KEY_CAPS_LOCK},
    {"SCROLLOCK", HID_KEY_SCROLL_LOCK},
    {"CONTROL", HID_KEY_CONTROL_LEFT},
    {"CTRL", HID_KEY_CONTROL_LEFT},
    {"SHIFT", HID_KEY_SHIFT_LEFT},
    {"ALT", HID_KEY_ALT_LEFT},
    {"WINDOWS", HID_KEY_GUI_LEFT},
    {"GUI", HID_KEY_GUI_LEFT}};
