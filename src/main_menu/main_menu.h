//
// Created by danie on 12/11/2021.
//

#ifndef KINCALC_MAIN_MENU_H
#define KINCALC_MAIN_MENU_H

#include "mm_eqs.h"

typedef struct {
    AllEqs *eqs;
    int selectedRow;
    int scroll;
    Param *editingParam;
} MMState;

void drawMainMenu(void);

#endif //KINCALC_MAIN_MENU_H
