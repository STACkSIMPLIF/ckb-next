#include "keymap.h"

// Normal size
#define NS 12, 12

#define KEYCOUNT_K95_104 133
#define KEYCOUNT_K95_105 134

KeyPos K95PosDE[KEYCOUNT_K95_105] = {
    {0, "mr", 38, 0, NS}, {0, "m1", 50, 0, NS}, {0, "m2", 62, 0, NS}, {0, "m3", 74, 0, NS}, {"Brightness", "light", 222, 0, NS}, {"Windows Lock", "lock", 234, 0, NS}, {"Mute", "mute", 273, 0, 13, 8},
    {0, "g1", 0, 14, NS}, {0, "g2", 11, 14, NS}, {0, "g3", 22, 14, NS}, {"Esc", "esc", 38, 14, NS}, {0, "f1", 58, 14, NS}, {0, "f2", 70, 14, NS}, {0, "f3", 82, 14, NS}, {0, "f4", 94, 14, NS}, {0, "f5", 114, 14, NS}, {0, "f6", 126, 14, NS}, {0, "f7", 138, 14, NS}, {0, "f8", 150, 14, NS}, {0, "f9", 170, 14, NS}, {0, "f10", 182, 14, NS}, {0, "f11", 194, 14, NS}, {0, "f12", 206, 14, NS}, {"Print Screen\nSysRq", "prtscn", 222, 14, NS}, {"Scroll Lock", "scroll", 234, 14, NS}, {"Pause\nBreak", "pause", 246, 14, NS}, {"Stop", "stop", 262, 14, 12, 8}, {"Previous", "prev", 273, 14, 13, 8}, {"Play/Pause", "play", 285, 14, 13, 8}, {"Next", "next", 296, 14, 12, 8},
    {0, "g4", 0, 25, NS}, {0, "g5", 11, 25, NS}, {0, "g6", 22, 25, NS}, {"^", "caret", 38, 27, NS}, {0, "1", 50, 27, NS}, {0, "2", 62, 27, NS}, {0, "3", 74, 27, NS}, {0, "4", 86, 27, NS}, {0, "5", 98, 27, NS}, {0, "6", 110, 27, NS}, {0, "7", 122, 27, NS}, {0, "8", 134, 27, NS}, {0, "9", 146, 27, NS}, {0, "0", 158, 27, NS}, {"ß", "ss", 170, 27, NS}, {"´", "grave", 182, 27, NS}, {"Backspace", "bspace", 200, 27, 24, 12}, {"Insert", "ins", 222, 27, NS}, {"Home", "home", 234, 27, NS}, {"Page Up", "pgup", 246, 27, NS}, {"Num Lock", "numlock", 261, 27, NS}, {"NumPad /", "numslash", 273, 27, NS}, {"NumPad *", "numstar", 285, 27, NS}, {"NumPad -", "numminus", 297, 27, NS},
    {0, "g7", 0, 39, NS}, {0, "g8", 11, 39, NS}, {0, "g9", 22, 39, NS}, {"Tab", "tab", 41, 39, 18, 12}, {0, "q", 56, 39, NS}, {0, "w", 68, 39, NS}, {0, "e", 80, 39, NS}, {0, "r", 92, 39, NS}, {0, "t", 104, 39, NS}, {0, "z", 116, 39, NS}, {0, "u", 128, 39, NS}, {0, "i", 140, 39, NS}, {0, "o", 152, 39, NS}, {0, "p", 164, 39, NS}, {"Ü", "ue", 176, 39, NS}, {"+", "plus", 188, 39, NS}, {"Enter", "enter", 203, 39, 18, 24}, {"Delete", "del", 222, 39, NS}, {"End", "end", 234, 39, NS}, {"Page Down", "pgdn", 246, 39, NS}, {"NumPad 7", "num7", 261, 39, NS}, {"NumPad 8", "num8", 273, 39, NS}, {"NumPad 9", "num9", 285, 39, NS}, {"NumPad +", "numplus", 297, 45, 12, 24},
    {0, "g10", 0, 50, NS}, {0, "g11", 11, 50, NS}, {0, "g12", 22, 50, NS}, {"Caps Lock", "caps", 42, 51, 20, 12}, {0, "a", 59, 51, NS}, {0, "s", 71, 51, NS}, {0, "d", 83, 51, NS}, {0, "f", 95, 51, NS}, {0, "g", 107, 51, NS}, {0, "h", 119, 51, NS}, {0, "j", 131, 51, NS}, {0, "k", 143, 51, NS}, {0, "l", 155, 51, NS}, {"Ö", "oe", 167, 51, NS}, {"Ä", "ae", 179, 51, NS}, {"#", "hash", 191, 51, NS}, {"NumPad 4", "num4", 261, 51, NS}, {"NumPad 5", "num5", 273, 51, NS}, {"NumPad 6", "num6", 285, 51, NS},
    {0, "g13", 0, 64, NS}, {0, "g14", 11, 64, NS}, {0, "g15", 22, 64, NS}, {"Left Shift", "lshift", 39, 63, 14, 12}, {"<", "angle", 53, 63, NS}, {0, "y", 65, 63, NS}, {0, "x", 77, 63, NS}, {0, "c", 89, 63, NS}, {0, "v", 101, 63, NS}, {0, "b", 113, 63, NS}, {0, "n", 125, 63, NS}, {0, "m", 137, 63, NS}, {",", "comma", 149, 63, NS}, {".", "dot", 161, 63, NS}, {"-", "minus", 173, 63, NS}, {"Right Shift", "rshift", 196, 63, 32, 12}, {"Up", "up", 234, 63, NS}, {"NumPad 1", "num1", 261, 63, NS}, {"NumPad 2", "num2", 273, 63, NS}, {"NumPad 3", "num3", 285, 63, NS}, {"NumPad Enter", "numenter", 297, 69, 12, 24},
    {0, "g16", 0, 75, NS}, {0, "g17", 11, 75, NS}, {0, "g18", 22, 75, NS}, {"Left Ctrl", "lctrl", 40, 75, 16, 12}, {"Left Windows", "lwin", 54, 75, NS}, {"Left Alt", "lalt", 67, 75, 14, 12}, {"Space", "space", 116, 75, 84, 12}, {"Right Alt", "ralt", 165, 75, 14, 12}, {"Right Windows", "rwin", 178, 75, NS}, {"Menu", "rmenu", 190, 75, NS}, {"Right Ctrl", "rctrl", 204, 75, 16, 12}, {"Left", "left", 222, 75, NS}, {"Down", "down", 234, 75, NS}, {"Right", "right", 246, 75, NS}, {"NumPad 0", "num0", 267, 75, 24, 12}, {"NumPad .", "numdot", 285, 75, NS},
};

