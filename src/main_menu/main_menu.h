//
// Created by danie on 12/11/2021.
//

#ifndef KINCALC_MAIN_MENU_H
#define KINCALC_MAIN_MENU_H

#include <fileioc.h>
#include "mm_eqs.h"

#define PROGRAM_STATE_APP_VAR_NAME "KINCALCS"

typedef enum {
    VAR_A = 0, VAR_C_OFFSET, VARS_A_AND_C, VAR_C_CONST
} VarsToGet;

typedef struct {
    AllEqs eqs;
    int selectedRow;
    int scroll;
    /**
     * -1 for none;
     */
    int editingVar;
    VarsToGet varsToGet;
} MMState;

MMState *initMMState(ti_var_t file);

void drawMainMenu(MMState *state);

#endif //KINCALC_MAIN_MENU_H
