//
// Created by danie on 12/12/2021.
//

#include "call_ti_basic.h"

#include <graphx.h>
#include <fileioc.h>
#include "init_font.h"
#include <debug.h>

int callback(void *data, int returnValue) {
    gfx_Begin();
    initFont();

    ti_var_t stateVar = ti_Open(PROGRAM_STATE_APP_VAR_NAME, "r");
    if(!stateVar) return 1;

    MMState *state = ti_GetDataPtr(stateVar);
    VariableValue *var = &state->eqs.variables[state->editingVar];
    VarsToGet vars = state->varsToGet;

    if (var->status == CONSTANT) {
        os_GetRealVar("B", &var->value);
    } else {
        if (vars == VAR_A || vars == VARS_A_AND_B) os_GetRealVar("A", &var->eq.coeff);
        if (vars == VAR_B || vars == VARS_A_AND_B) os_GetRealVar("B", &var->eq.intercept);
    }

    drawMainMenu(state);

    ti_Close(stateVar);
    os_DelAppVar(PROGRAM_STATE_APP_VAR_NAME);
    os_DelAppVar("OTINSA");
    gfx_End();

    return 0;
}

void tiBasicA(MMState *state) {
    gfx_End();

    const char code[] = "\xE1\x3F\xDD\x41";
    const char name[] = "OTINSA";

    ti_var_t program = ti_OpenVar(name, "w", TI_PRGM_TYPE);
    ti_Write(code, sizeof code, 1, program);
    ti_Close(program);

    state->varsToGet = VAR_A;
    os_RunPrgm(name, NULL, 0, callback);
}

void tiBasicB(MMState *state) {
    gfx_End();

    const char code[] = "\xE1\x3F\xDD\x42";
    const char name[] = "OTINSA";

    ti_var_t program = ti_OpenVar(name, "w", TI_PRGM_TYPE);
    ti_Write(code, sizeof code, 1, program);
    ti_Close(program);

    state->varsToGet = VAR_B;
    os_RunPrgm(name, NULL, 0, callback);
}

void tiBasicAAndB(MMState *state) {
    gfx_End();

    const char code[] = "\xE1\x3F\xDD\x41\x2B\x42";
    const char name[] = "OTINSA";

    ti_var_t program = ti_OpenVar(name, "w", TI_PRGM_TYPE);
    ti_Write(code, sizeof code, 1, program);
    ti_Close(program);

    state->varsToGet = VARS_A_AND_B;
    os_RunPrgm(name, NULL, 0, callback);
}
