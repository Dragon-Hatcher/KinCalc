//
// Created by danie on 12/11/2021.
//

#include "menu.h"

#include <graphx.h>
#include "bottom_menu.h"
#include "draw_simple_text.h"
#include "init_font.h"

#define ROWS 10

static void redraw(const char *items[], int length, int offset, int select, int except) {
    for (int row = 1, item = offset; row <= 10 && item < length ; row++, item++) {
        txt_writeStrAtPos(0, row, select == item ? "\x0F" : " ");
        txt_writeStrAtPos(1, row, items[item]);
        if (item == except) {
            int y = START_HEIGHT + LINE_HEIGHT * row + 8;
            int x1, x2;
            if (items[item][3] == ' ') {
                x1 = START_WIDTH + CHAR_WIDTH * 1 - 2;
                x2 = START_WIDTH + CHAR_WIDTH * (items[item][1] == ' ' ? 2 : 3) - 1;
            } else {
                x1 = START_WIDTH + CHAR_WIDTH * 3.5;
                x2 = START_WIDTH + CHAR_WIDTH * 6.5 - 1;
                if (items[item][4] == ' ') x2 -= CHAR_WIDTH;
            }
            gfx_SetColor(0xC0);
            gfx_Line(x1, y, x2, y);
            gfx_Line(x1, y + 1, x2, y + 1);
            gfx_SetColor(0);
        }
    }
}

int menuExcept(const char* title, const char *items[], int length, int except) {
    gfx_FillScreen(0xFF);

    bottomMenu(3, 2, "CANCEL");

    txt_writeStrAtPos(0, 0, title);

    int selectedItem = 0;
    int scroll = 0;
    redraw(items, length, scroll, selectedItem, except);

    sk_key_t key = os_GetCSC();
    while ((key != sk_Enter || selectedItem == except) && key != sk_Graph && key != sk_Trace) {
        if (key == sk_Up && selectedItem != 0) {
            selectedItem--;
            if (selectedItem < scroll) scroll = selectedItem;
            redraw(items, length, scroll, selectedItem, except);
        } else if (key == sk_Down && selectedItem != length - 1) {
            selectedItem++;
            if (selectedItem >= scroll + ROWS) scroll++;
            redraw(items, length, scroll, selectedItem, except);
        }
        key = os_GetCSC();
    }

    gfx_FillScreen(0xFF);
    return key == sk_Enter ? selectedItem : -1;
}

int menu(const char* title, const char *items[], int length) {
    return menuExcept(title, items, length, -1);
}