//
// Created by danie on 12/11/2021.
//

#include "init_font.h"

#include <fontlibc.h>

int initFont(void) {
    fontlib_font_t *my_font = fontlib_GetFontByIndex("OSLFONT", 0);
    if (!my_font) return 1;
    fontlib_SetFont(my_font, 0);
    fontlib_SetColors(0x00, 0xFF);
    return 0;
}