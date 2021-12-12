//
// Created by danie on 12/11/2021.
//

#include "main_menu.h"

#include <tice.h>
#include <graphx.h>
#include "bottom_menu.h"
#include "new_eq.h"
#include "draw_simple_text.h"

static void solve(void) {}

static void redraw(AllEqs *eqs) {
    gfx_FillScreen(0xFF);
    bottomMenu(0, 1, "QUIT");
    bottomMenu(1, 2, "NEW EQ");
    bottomMenu(3, 2, "SOLVE");

    int row = 0;
    for (int i = 0; i < eqs->freeVars->count; i++, row++) {
        char *str = eqs->freeVars->vars[i].name;
        drawAtCharPos(0, row, str);
    }
}

void drawMainMenu(void) {
    static FreeVars freeVars = { .count = 0 };
    static AllEqs eqs = {
        .freeVars = &freeVars
    };

    redraw(&eqs);

    do {
        sk_key_t key = os_GetCSC();
        if (key == sk_Yequ) break;
        if (key == sk_Window || key == sk_Zoom) { newEq(&eqs); redraw(&eqs); }
        if (key == sk_Trace || key == sk_Graph) { solve(); redraw(&eqs); }

    } while (true);
}