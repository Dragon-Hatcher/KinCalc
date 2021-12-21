//
// Created by danie on 12/12/2021.
//

#include "call_ti_basic.h"

#include <graphx.h>
#include <fileioc.h>
#include "init_font.h"

#define GETTER_PROGRAM_NAME "KINCALCG"

int callback(__attribute__((unused)) void *data, __attribute__((unused)) int returnValue) {
    gfx_Begin();
    initFont();
    ti_DeleteVar(GETTER_PROGRAM_NAME, TI_PPRGM_TYPE);

    ti_var_t stateVar = ti_Open(PROGRAM_STATE_APP_VAR_NAME, "r");
    if (!stateVar) return 1;

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
    gfx_End();

    return 0;
}

const char codeA[] = {tClLCD, tEnter, tPrompt, tA};
const char codeB[] = {tClLCD, tEnter, tPrompt, tB};
const char codeAB[] = {tClLCD, tEnter, tPrompt, tA, tComma, tB};
const char *codes[] = {codeA, codeB, codeAB};
const int codeSizes[] = {sizeof codeA, sizeof codeB, sizeof codeAB};

void getVariableInput(MMState *state, VarsToGet vars) {
    state->varsToGet = vars;

    gfx_End();

    if (vars == VAR_A || vars == VARS_A_AND_B) ti_DeleteVar("A", TI_REAL_TYPE);
    if (vars == VAR_B || vars == VARS_A_AND_B) ti_DeleteVar("B", TI_REAL_TYPE);

    ti_var_t program = ti_OpenVar(GETTER_PROGRAM_NAME, "w", TI_PPRGM_TYPE);
    ti_Write(codes[vars], codeSizes[vars], 1, program);
    ti_Close(program);

    os_RunPrgm(GETTER_PROGRAM_NAME, NULL, 0, callback);
}
