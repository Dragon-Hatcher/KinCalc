//
// Created by danie on 12/11/2021.
//

#include "draw_simple_text.h"

#include <fontlibc.h>
#include "init_font.h"

void drawAtCharPos(int charX, int charY, const char* str) {
    int x = START_WIDTH + CHAR_WIDTH * charX;
    int y = START_HEIGHT + charY * LINE_HEIGHT;
    fontlib_SetCursorPosition(x, y);
    fontlib_DrawString(str);
}
