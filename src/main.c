//
// Created by daniel on 12/11/2021.
//

#include <tice.h>
#include <graphx.h>
#include <fileioc.h>
#include "init_font.h"
#include "main_menu/main_menu.h"
#include "main_menu/prg_state_in_appvar.h"

#define EXIT_ERROR(str) do { \
    gfx_End();               \
    os_ClrHomeFull();        \
    os_PutStrFull(str);      \
    while(!os_GetCSC());     \
    return 1;                \
} while(0);

int main(void) {
    gfx_Begin();

    if(initFont()) EXIT_ERROR("Missing font OSLFONT.");
    ti_var_t stateVar = ti_Open(PROGRAM_STATE_APP_VAR_NAME, "w");
    if(!stateVar) EXIT_ERROR("Could not open appvar");
    MMState *state = initMMState(stateVar);
    ti_Close(stateVar);

    drawMainMenu(state, false);

    gfx_End();
}
