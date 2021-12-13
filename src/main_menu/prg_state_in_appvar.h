//
// Created by danie on 12/12/2021.
//

#ifndef KINCALC_PRG_STATE_IN_APPVAR_H
#define KINCALC_PRG_STATE_IN_APPVAR_H

#include <fileioc.h>
#include "main_menu.h"

#define PROGRAM_STATE_APP_VAR_NAME "DLNNP"

MMState *initMMState(ti_var_t stateVar);

#endif //KINCALC_PRG_STATE_IN_APPVAR_H
