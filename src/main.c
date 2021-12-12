//
// Created by daniel on 12/11/2021.
//

#include <tice.h>
#include <graphx.h>
#include "init_font.h"
#include "main_menu/main_menu.h"

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

    drawMainMenu();

    gfx_End();
}
