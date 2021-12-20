//
// Created by danie on 12/11/2021.
//

#include "menu.h"

#include <graphx.h>
#include "bottom_menu.h"
#include "draw_simple_text.h"

#define ROWS 10

static void redraw(const char *items[], int length, int offset, int select) {
    for (int row = 0, item = offset; row < 10 && item < length ; row++, item++) {
        txt_writeStrAtPos(0, row + 1, select == item ? "\x0F" : " ");
        txt_writeStrAtPos(1, row + 1, items[item]);
    }
}

int menu(const char* title, const char *items[], int length) {
    gfx_FillScreen(0xFF);

    bottomMenu(3, 2, "CANCEL");

    txt_writeStrAtPos(0, 0, title);

    int selectedItem = 0;
    int scroll = 0;
    redraw(items, length, scroll, selectedItem);

    sk_key_t key = os_GetCSC();
    while (key != sk_Enter && key != sk_Graph && key != sk_Trace) {
        if (key == sk_Up && selectedItem != 0) {
            selectedItem--;
            if (selectedItem < scroll) scroll = selectedItem;
            redraw(items, length, scroll, selectedItem);
        } else if (key == sk_Down && selectedItem != length - 1) {
            selectedItem++;
            if (selectedItem >= scroll + ROWS) scroll++;
            redraw(items, length, scroll, selectedItem);
        }
        key = os_GetCSC();
    }

    gfx_FillScreen(0xFF);
    return key == sk_Enter ? selectedItem : -1;
}