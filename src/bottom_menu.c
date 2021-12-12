//
// Created by daniel on 12/11/2021.
//

#include "bottom_menu.h"

#include <graphx.h>
#include <tice.h>
#include <fontlibc.h>
#include <string.h>
#include "init_font.h"

#define M_HEIGHT (16)
#define ML_THICKNESS (2)
#define M_SIDE_GAP (2)
#define M_WIDTH (((LCD_WIDTH - M_SIDE_GAP * 2) / 5) - 1 - ML_THICKNESS)

static void printTextCentered(const char *str, unsigned int x, unsigned int width) {
    unsigned int textWidth = strlen(str) * CHAR_WIDTH;
    if (textWidth > width) return;
    unsigned int cursorX = x + (width - textWidth) / 2;
    uint8_t cursorY = LCD_HEIGHT - CHAR_HEIGHT;
    fontlib_SetCursorPosition(cursorX, cursorY);
    fontlib_DrawString(str);
}

void bottomMenu(uint8_t pos, uint8_t cellWidth, const char* text) {
    int x = M_SIDE_GAP + ML_THICKNESS + pos * (M_WIDTH + ML_THICKNESS + 1);
    int y = LCD_HEIGHT - M_HEIGHT - ML_THICKNESS;

    //top line
    int width = M_WIDTH + (cellWidth - 1) * (M_WIDTH + ML_THICKNESS + 1);
    gfx_FillRectangle_NoClip(x, y, width, ML_THICKNESS);

    //left line
    gfx_FillRectangle_NoClip(x - ML_THICKNESS, y + ML_THICKNESS, ML_THICKNESS, M_HEIGHT);
    gfx_FillRectangle_NoClip(x - ML_THICKNESS + 1, y + 1, ML_THICKNESS, ML_THICKNESS);

    //right line
    gfx_FillRectangle_NoClip(x + width, y + ML_THICKNESS, ML_THICKNESS, M_HEIGHT);
    gfx_FillRectangle_NoClip(x + width - 1, y + 1, ML_THICKNESS, ML_THICKNESS);

    //text
    printTextCentered(text, x, width);
}