//
// Created by danie on 12/11/2021.
//

#include "menu.h"

#include <graphx.h>
#include "bottom_menu.h"
#include "draw_simple_text.h"

int menu(const char* title, const char *items[], int length) {
    gfx_FillScreen(0xFF);

    bottomMenu(3, 2, "CANCEL");

    drawAtCharPos(0, 0, title);
    for (int i = 0; i < length; i++) {
        drawAtCharPos(1, i + 1, items[i]);
    }

    int selectedItem = 0;
    drawAtCharPos(0, selectedItem + 1, "\x0F");
    sk_key_t key = os_GetCSC();
    while (key != sk_Enter && key != sk_Graph && key != sk_Trace) {
        if (key == sk_Up) {
            drawAtCharPos(0, selectedItem-- + 1, " ");
            if (selectedItem < 0) selectedItem = length - 1;
            drawAtCharPos(0, selectedItem + 1, "\x0F");
        } else if (key == sk_Down) {
            drawAtCharPos(0, selectedItem++ + 1, " ");
            if (selectedItem == length) selectedItem = 0;
            drawAtCharPos(0, selectedItem + 1, "\x0F");
        }
        key = os_GetCSC();
    }

    gfx_FillScreen(0xFF);
    return key == sk_Enter ? selectedItem : -1;
}