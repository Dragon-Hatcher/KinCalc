//
// Created by danie on 12/11/2021.
//

#include "menu.h"

#include <graphx.h>
#include <fontlibc.h>
#include "bottom_menu.h"
#include "init_font.h"

#define START_HEIGHT LINE_GAP
#define START_WIDTH CHAR_GAP

static void drawAtCharPos(int charX, int charY, char* str) {
    int x = START_WIDTH + CHAR_WIDTH * charX;
    int y = START_HEIGHT + charY * LINE_HEIGHT;
    fontlib_SetCursorPosition(x, y);
    fontlib_DrawString(str);
}

int menu(char* title, char *items[], int length) {
    gfx_FillScreen(0xFF);

    bottomMenu(3, 2, "CANCEL");

    drawAtCharPos(0, 0, title);
    for (int i = 0; i < length; i++) {
        drawAtCharPos(2, i + 1, items[i]);
    }

    int selectedItem = 0;
    drawAtCharPos(0, selectedItem + 1, ">");
    sk_key_t key = os_GetCSC();
    while (key != sk_Enter && key != sk_Graph && key != sk_Trace) {
        if (key == sk_Up && selectedItem != 0) {
            drawAtCharPos(0, selectedItem-- + 1, " ");
            drawAtCharPos(0, selectedItem + 1, ">");
        } else if (key == sk_Down && selectedItem != length - 1) {
            drawAtCharPos(0, selectedItem++ + 1, " ");
            drawAtCharPos(0, selectedItem + 1, ">");
        }
        key = os_GetCSC();
    }

    gfx_FillScreen(0xFF);
    return key == sk_Enter ? selectedItem : -1;
}