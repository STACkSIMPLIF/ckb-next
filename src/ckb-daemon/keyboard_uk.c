#include "keyboard.h"

const key keymap_uk[N_KEYS] = {
    { "esc",        0x00, KEY_ESC },
    { "f1",         0x0c, KEY_F1 },
    { "f2",         0x18, KEY_F2 },
    { "f3",         0x24, KEY_F3 },
    { "f4",         0x30, KEY_F4 },
    { "f5",         0x3c, KEY_F5 },
    { "f6",         0x48, KEY_F6 },
    { "f7",         0x54, KEY_F7 },
    { "f8",         0x60, KEY_F8 },
    { "f9",         0x6c, KEY_F9 },
    { "f10",        0x78, KEY_F10 },
    { "f11",        0x84, KEY_F11 },
    { "grave",      0x01, KEY_GRAVE_UK },
    { "1",          0x0d, KEY_1 },
    { "2",          0x19, KEY_2 },
    { "3",          0x25, KEY_3 },
    { "4",          0x31, KEY_4 },
    { "5",          0x3d, KEY_5 },
    { "6",          0x49, KEY_6 },
    { "7",          0x55, KEY_7 },
    { "8",          0x61, KEY_8 },
    { "9",          0x6d, KEY_9 },
    { "0",          0x79, KEY_0 },
    { "minus",      0x85, KEY_MINUS },
    { "tab",        0x02, KEY_TAB },
    { "q",          0x0e, KEY_Q },
    { "w",          0x1a, KEY_W },
    { "e",          0x26, KEY_E },
    { "r",          0x32, KEY_R },
    { "t",          0x3e, KEY_T },
    { "y",          0x4a, KEY_Y },
    { "u",          0x56, KEY_U },
    { "i",          0x62, KEY_I },
    { "o",          0x6e, KEY_O },
    { "p",          0x7a, KEY_P },
    { "lbrace",     0x86, KEY_LEFTBRACE },
    { "caps",       0x03, KEY_CAPSLOCK },
    { "a",          0x0f, KEY_A },
    { "s",          0x1b, KEY_S },
    { "d",          0x27, KEY_D },
    { "f",          0x33, KEY_F },
    { "g",          0x3f, KEY_G },
    { "h",          0x4b, KEY_H },
    { "j",          0x57, KEY_J },
    { "k",          0x63, KEY_K },
    { "l",          0x6f, KEY_L },
    { "colon",      0x7b, KEY_SEMICOLON },
    { "quote",      0x87, KEY_APOSTROPHE },
    { "lshift",     0x04, KEY_LEFTSHIFT },
    { "bslash",     0x10, KEY_102ND }, // Modified in UK (was 0, -1, -1). This is the backslash key, but using KEY_BACKSLASH gives a #.
    { "z",          0x1c, KEY_Z },
    { "x",          0x28, KEY_X },
    { "c",          0x34, KEY_C },
    { "v",          0x40, KEY_V },
    { "b",          0x4c, KEY_B },
    { "n",          0x58, KEY_N },
    { "m",          0x64, KEY_M },
    { "comma",      0x70, KEY_COMMA },
    { "dot",        0x7c, KEY_DOT },
    { "slash",      0x88, KEY_SLASH },
    { "lctrl",      0x05, KEY_LEFTCTRL },
    { "lwin",       0x11, KEY_LEFTMETA },
    { "lalt",       0x1d, KEY_LEFTALT },
    { 0,            -1, KEY_NONE },
    { "space",      0x35, KEY_SPACE },
    { 0,            -1, KEY_NONE },
    { 0,            -1, KEY_NONE },
    { "ralt",       0x59, KEY_RIGHTALT },
    { "rwin",       0x65, KEY_RIGHTMETA },
    { "rmenu",      0x71, KEY_COMPOSE },
    { 0,            -1, KEY_NONE },
    { "light",      0x89, KEY_CORSAIR },
    { "f12",        0x06, KEY_F12 },
    { "prtscn",     0x12, KEY_SYSRQ },
    { "scroll",     0x1e, KEY_SCROLLLOCK },
    { "pause",      0x2a, KEY_PAUSE },
    { "ins",        0x36, KEY_INSERT },
    { "home",       0x42, KEY_HOME },
    { "pgup",       0x4e, KEY_PAGEUP },
    { "rbrace",     0x5a, KEY_RIGHTBRACE },
    { 0,            -1, KEY_NONE }, // Modified in UK (was backslash)
    { "hash",       0x72, KEY_BACKSLASH }, // Modified in UK (was 0, -1, -1). This is the # key.
    { "enter",      0x7e, KEY_ENTER },
    { 0,            -1, KEY_NONE },
    { "equal",      0x07, KEY_EQUAL },
    { 0,            -1, KEY_NONE },
    { "bspace",     0x1f, KEY_BACKSPACE },
    { "del",        0x2b, KEY_DELETE },
    { "end",        0x37, KEY_END },
    { "pgdn",       0x43, KEY_PAGEDOWN },
    { "rshift",     0x4f, KEY_RIGHTSHIFT },
    { "rctrl",      0x5b, KEY_RIGHTCTRL },
    { "up",         0x67, KEY_UP },
    { "left",       0x73, KEY_LEFT },
    { "down",       0x7f, KEY_DOWN },
    { "right",      0x8b, KEY_RIGHT },
    { "lock",       0x08, KEY_CORSAIR },
    { "mute",       0x14, KEY_MUTE },
    { "stop",       0x20, KEY_STOP },
    { "prev",       0x2c, KEY_PREVIOUSSONG },
    { "play",       0x38, KEY_PLAYPAUSE },
    { "next",       0x44, KEY_NEXTSONG },
    { "numlock",    0x50, KEY_NUMLOCK },
    { "numslash",   0x5c, KEY_KPSLASH },
    { "numstar",    0x68, KEY_KPASTERISK },
    { "numminus",   0x74, KEY_KPMINUS },
    { "numplus",    0x80, KEY_KPPLUS },
    { "numenter",   0x8c, KEY_KPENTER },
    { "num7",       0x09, KEY_KP7 },
    { "num8",       0x15, KEY_KP8 },
    { "num9",       0x21, KEY_KP9 },
    { 0,            -1, KEY_NONE },
    { "num4",       0x39, KEY_KP4 },
    { "num5",       0x45, KEY_KP5 },
    { "num6",       0x51, KEY_KP6 },
    { "num1",       0x5d, KEY_KP1 },
    { "num2",       0x69, KEY_KP2 },
    { "num3",       0x75, KEY_KP3 },
    { "num0",       0x81, KEY_KP0 },
    { "numdot",     0x8d, KEY_KPDOT },
    { "g1",         0x0a, KEY_CORSAIR },
    { "g2",         0x16, KEY_CORSAIR },
    { "g3",         0x22, KEY_CORSAIR },
    { "g4",         0x2e, KEY_CORSAIR },
    { "g5",         0x3a, KEY_CORSAIR },
    { "g6",         0x46, KEY_CORSAIR },
    { "g7",         0x52, KEY_CORSAIR },
    { "g8",         0x5e, KEY_CORSAIR },
    { "g9",         0x6a, KEY_CORSAIR },
    { "g10",        0x76, KEY_CORSAIR },
    { "volup",      -1, KEY_VOLUMEUP },
    { "voldn",      -1, KEY_VOLUMEDOWN },
    { "mr",         0x0b, KEY_CORSAIR },
    { "m1",         0x17, KEY_CORSAIR },
    { "m2",         0x23, KEY_CORSAIR },
    { "m3",         0x2f, KEY_CORSAIR },
    { "g11",        0x3b, KEY_CORSAIR },
    { "g12",        0x47, KEY_CORSAIR },
    { "g13",        0x53, KEY_CORSAIR },
    { "g14",        0x5f, KEY_CORSAIR },
    { "g15",        0x6b, KEY_CORSAIR },
    { "g16",        0x77, KEY_CORSAIR },
    { "g17",        0x83, KEY_CORSAIR },
    { "g18",        0x8f, KEY_CORSAIR }
};
