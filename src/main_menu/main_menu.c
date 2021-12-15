//
// Created by danie on 12/11/2021.
//

#include "main_menu.h"

#include <tice.h>
#include <graphx.h>
#include "bottom_menu.h"
#include "new_eq.h"
#include "draw_simple_text.h"
#include "new_param.h"

static void solve(void) {}

#define CURSOR_CHAR "\x0F"
#define EMPTY_ROW "                          "

#define SCREEN_LINES 10

#define ACC_ROWS 6
#define VEL_ROWS 4
#define VEL_SUM_ROWS 3
#define FREE_VAR_ROWS 1

static int rowCount(AllEqs *eqs) {
    return eqs->accCount * ACC_ROWS + eqs->velCount * VEL_ROWS + eqs->velSumCount * VEL_SUM_ROWS +
           eqs->freeVarCount * FREE_VAR_ROWS;
}

static void drawCursor(MMState *state, const char *cursorChar) {
    if (state->selectedRow >= rowCount(&state->eqs)) return;

    int screenRow = state->selectedRow - state->scroll;
    bool isIndented;

    {
        int selectedRow = state->selectedRow;
        if (selectedRow < state->eqs.freeVarCount * FREE_VAR_ROWS) {
            isIndented = false;
        } else {
            selectedRow -= state->eqs.freeVarCount * FREE_VAR_ROWS;
            if (selectedRow < state->eqs.velSumCount * VEL_SUM_ROWS) {
                isIndented = selectedRow % VEL_SUM_ROWS != 0;
            } else {
                selectedRow -= state->eqs.freeVarCount * VEL_SUM_ROWS;
                if (selectedRow < state->eqs.velCount * VEL_ROWS) {
                    isIndented = selectedRow % VEL_ROWS != 0;
                } else {
                    selectedRow -= state->eqs.velCount * VEL_ROWS;
                    isIndented = selectedRow % ACC_ROWS != 0;
                }
            }
        }
    }

    drawAtCharPos(isIndented ? 1 : 0, screenRow, cursorChar);
}

#define DRAW_ROW(S) do {                   \
    if (screenRow > SCREEN_LINES) break;   \
    if (rowsPassed++ >= state->scroll) {   \
        drawAtCharPos(1, screenRow++, S);  \
    }                                      \
} while (0);

#define DRAW_ROW_VALUE(S, T, NUM, F) do {                                               \
    if (screenRow > SCREEN_LINES) break;                                                \
    if (rowsPassed++ >= state->scroll) {                                                \
        drawAtCharPos(1, screenRow, S);                                                 \
        drawAtCharPos(5, screenRow, "=");                                               \
        char buffer[DESCRIPTION_SIZE];                                                  \
        variableDescription(&state->eqs, eqForField(&state->eqs, T, NUM, F), buffer);   \
        drawAtCharPos(7, screenRow++, buffer);                                          \
    }                                                                                   \
} while (0);

static void drawRows(MMState *state) {
    for (int i = 0; i < SCREEN_LINES; i++) {
        drawAtCharPos(0, i, EMPTY_ROW);
    }

    int screenRow = 0;
    int rowsPassed = 0;

    for (int i = 0; i < state->eqs.freeVarCount; i++) {
        DRAW_ROW_VALUE(state->eqs.freeVarNames[i], FREE_VAR, i, VAR)
    }
    for (int i = 0; i < state->eqs.velSumCount; i++) {
        DRAW_ROW_VALUE(state->eqs.velSumNames[i], VEL, i, SUM_V)
        DRAW_ROW_VALUE(" vx", VEL, i, VX)
        DRAW_ROW_VALUE(" vy", VEL, i, VY)
    }
    for (int i = 0; i < state->eqs.velCount; i++) {
        DRAW_ROW(state->eqs.velNames[i])
        DRAW_ROW_VALUE(" \x16x", VEL, i, DX)
        DRAW_ROW_VALUE(" \x16t", VEL, i, DT)
        DRAW_ROW_VALUE(" v", VEL, i, VEL_V)
    }
    for (int i = 0; i < state->eqs.accCount; i++) {
        DRAW_ROW(state->eqs.accNames[i])
        DRAW_ROW_VALUE(" \x16x", ACC, i, DX)
        DRAW_ROW_VALUE(" \x16t", ACC, i, DT)
        DRAW_ROW_VALUE(" v0", ACC, i, V0)
        DRAW_ROW_VALUE(" v", ACC, i, ACC_V)
        DRAW_ROW_VALUE(" a", ACC, i, A)
    }
}

static void drawMenus() {
    bottomMenu(0, 1, "QUIT");
    bottomMenu(1, 2, "NEW EQ");
    bottomMenu(3, 2, "SOLVE");
}

static void fullRedraw(MMState *state) {
    state->scroll = 0;
    state->selectedRow = 0;
    gfx_FillScreen(0xFF);
    drawMenus();
    drawRows(state);
    drawCursor(state, CURSOR_CHAR);
}

static bool fixScroll(MMState *state) {
    if (state->selectedRow < state->scroll) {
        state->scroll = state->selectedRow;
        return true;
    }
    if (state->selectedRow > (state->scroll + SCREEN_LINES)) {
        state->scroll = state->selectedRow - SCREEN_LINES;
        return true;
    }
    return false;
}

static void doScroll(MMState *state, bool up) {
    if (up && state->selectedRow != 0) {
        drawCursor(state, " ");
        state->selectedRow--;
        if (fixScroll(state)) drawRows(state);
        drawCursor(state, CURSOR_CHAR);
    } else if (!up && state->selectedRow != rowCount(&state->eqs) - 1) {
        drawCursor(state, " ");
        state->selectedRow++;
        if (fixScroll(state)) drawRows(state);
        drawCursor(state, CURSOR_CHAR);
    }
}

void drawMainMenu(MMState *state) {
    fullRedraw(state);

    do {
        sk_key_t key = os_GetCSC();
        if (key == sk_Yequ) break;
        if (key == sk_Window || key == sk_Zoom) {
            newEq(&state->eqs);
            fullRedraw(state);
        }
        if (key == sk_Trace || key == sk_Graph) {
            solve();
            state->selectedRow = 0;
            fullRedraw(state);
        }
        if (key == sk_Up) doScroll(state, true);
        if (key == sk_Down) doScroll(state, false);

//        if (key == sk_Enter) {
//            if (checkForNewParam(state)) {}
//            redraw(state);
//        }
    } while (true);
}