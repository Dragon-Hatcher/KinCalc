//
// Created by danie on 12/11/2021.
//

#ifndef KINCALC_MAIN_MENU_H
#define KINCALC_MAIN_MENU_H

#include <fileioc.h>
#include "mm_eqs.h"

typedef struct {
    AllEqs eqs;
    int selectedRow;
    int scroll;
    /**
     * -1 for none;
     */
    int editingVar;
} MMState;

MMState *initMMState(ti_var_t file);

void drawMainMenu(MMState *state);

#endif //KINCALC_MAIN_MENU_H
