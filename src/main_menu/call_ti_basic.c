//
// Created by danie on 12/12/2021.
//

#include "call_ti_basic.h"

#include <graphx.h>
#include <fileioc.h>
#include "init_font.h"
#include "prg_state_in_appvar.h"
#include <debug.h>

static void loadA(MMState *state) {
//    ti_var_t a = ti_OpenVar("A", "r", TI_REAL_TYPE);
//    real_t aValue;
//    ti_Close(a);
//    ti_Read(&aValue, sizeof(real_t), 1, a);
//    state->editingParam->coeff = aValue;
}

static void loadB(MMState *state) {
//    ti_var_t b = ti_OpenVar("B", "r", TI_REAL_TYPE);
//    real_t bValue;
//    ti_Read(&bValue, sizeof(real_t), 1, b);
//    ti_Close(b);
//    state->editingParam->offset = bValue;
}

int callback(void *data, int returnValue) {
//    gfx_Begin();
//    initFont();
//
//    while (!os_GetCSC());
//
//    ti_var_t stateVar = ti_Open(PROGRAM_STATE_APP_VAR_NAME, "r");
//    if(!stateVar) return 1;
//    MMState *state = ti_GetDataPtr(stateVar) + 2;
//
//    dbg_sprintf(dbgout, "callback: %p\n", state);
//
//    ParamType type = state->editingParam->type;
//
//    if (type == COEFF || type == LINEAR) loadA(state);
//    if (type == CONSTANT || type == OFFSET || type == LINEAR) loadB(state);
//
//    drawMainMenu(data, true);
//
//    ti_Close(stateVar);
//
//    gfx_End();
//
    return 0;
}

void tiBasicA(MMState *state) {
//    gfx_End();
//
//    const char code[] = "\xE1\x3F\xDD\x41";
//    const char name[] = "OTINSA";
//
//    ti_var_t program = ti_OpenVar(name, "w", TI_PRGM_TYPE);
//    ti_Write(code, sizeof code, 1, program);
//    ti_Close(program);
//
//    os_RunPrgm(name, state, sizeof state, callback);
}

void tiBasicB(MMState *state) {
//    gfx_End();
//
//    const char code[] = "\xE1\x3F\xDD\x42";
//    const char name[] = "OTINSA";
//
//    ti_var_t program = ti_OpenVar(name, "w", TI_PRGM_TYPE);
//    ti_Write(code, sizeof code, 1, program);
//    ti_Close(program);
//
//    os_RunPrgm(name, state, sizeof state, callback);
}

void tiBasicAAndB(MMState *state) {
//    gfx_End();
//
//    const char code[] = "\xE1\x3F\xDD\x41\x2B\x42";
//    const char name[] = "OTINSA";
//
//    ti_var_t program = ti_OpenVar(name, "w", TI_PRGM_TYPE);
//    ti_Write(code, sizeof code, 1, program);
//    ti_Close(program);
//
//    os_RunPrgm(name, state, sizeof state, callback);
}
