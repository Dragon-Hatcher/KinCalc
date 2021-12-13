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

#define SCREEN_LINES 10
#define INC_ROW(x) do {                          \
    if (rowsPassed >= state->scroll) {           \
        x;                                       \
        if(++drawRow > SCREEN_LINES) return;     \
    }                                            \
    rowsPassed++;                                \
} while(0);

static void printRow(int indent, int row, char *str, bool isSelectedRow) {
    drawAtCharPos(indent + 1, row, str);
    if (isSelectedRow) drawAtCharPos(indent, row, "\x0F");
}

static void redraw(MMState *state) {
    gfx_FillScreen(0xFF);
    bottomMenu(0, 1, "QUIT");
    bottomMenu(1, 2, "NEW EQ");
    bottomMenu(3, 2, "SOLVE");

    AllEqs *eqs = state->eqs;
    int selectedRow = state->selectedRow;
    int rowsPassed = 0;
    int drawRow = 0;
    for (int i = 0; i < eqs->freeVars->count; i++) {
        INC_ROW(printRow(0, drawRow, eqs->freeVars->vars[i].name, rowsPassed == selectedRow));
    }
    for (int i = 0; i < eqs->velSums->count; i++) {
        VelSum velSum = eqs->velSums->sums[i];
        INC_ROW(printRow(0, drawRow, velSum.name, rowsPassed == selectedRow));
        INC_ROW(printRow(1, drawRow, "x: ", rowsPassed == selectedRow));
        INC_ROW(printRow(1, drawRow, "y: ", rowsPassed == selectedRow));
    }
    for (int i = 0; i < eqs->vels->count; i++) {
        ConstVel vel = eqs->vels->vels[i];
        INC_ROW(printRow(0, drawRow, vel.name, rowsPassed == selectedRow));
        INC_ROW(printRow(1, drawRow, "\x16x: ", rowsPassed == selectedRow));
        INC_ROW(printRow(1, drawRow, "\x16t: ", rowsPassed == selectedRow));
        INC_ROW(printRow(1, drawRow, "v: ", rowsPassed == selectedRow));
    }
    for (int i = 0; i < eqs->accs->count; i++) {
        ConstAcc acc = eqs->accs->accs[i];
        INC_ROW(printRow(0, drawRow, acc.name, rowsPassed == selectedRow));
        INC_ROW(printRow(1, drawRow, "\x16x: ", rowsPassed == selectedRow));
        INC_ROW(printRow(1, drawRow, "\x16t: ", rowsPassed == selectedRow));
        INC_ROW(printRow(1, drawRow, "v0: ", rowsPassed == selectedRow));
        INC_ROW(printRow(1, drawRow, "v: ", rowsPassed == selectedRow));
        INC_ROW(printRow(1, drawRow, "a: ", rowsPassed == selectedRow));
    }
}

static int rowCount(AllEqs *eqs) {
    return eqs->accs->count * 6 + eqs->vels->count * 4 + eqs->velSums->count * 3 + eqs->freeVars->count;
}

static void fixScroll(MMState *state) {
    if (state->selectedRow < state->scroll) state->scroll = state->selectedRow;
    if (state->selectedRow > (state->scroll + SCREEN_LINES)) state->scroll = state->selectedRow - SCREEN_LINES;
}

#define DO_NEW_PARAM(x) do { \
    newParam(x, state);      \
    return true;             \
} while(0);

static bool checkForNewParam(MMState *state) {
    AllEqs *eqs = state->eqs;
    int row = state->selectedRow;
    if (row >= eqs->freeVars->count) row -= eqs->freeVars->count;
    for (int i = 0; i < eqs->velSums->count; i++) {
        if (row >= 3) {
            row -= 3;
            continue;
        } else {
            if (row == 0) return false;
            if (row == 1) DO_NEW_PARAM(&eqs->velSums->sums[i].x)
            if (row == 2) DO_NEW_PARAM(&eqs->velSums->sums[i].y)
        }
    }
    for (int i = 0; i < eqs->vels->count; i++) {
        if (row >= 4) {
            row -= 4;
            continue;
        } else {
            if (row == 0) return false;
            if (row == 1) DO_NEW_PARAM(&eqs->vels->vels[i].dx)
            if (row == 2) DO_NEW_PARAM(&eqs->vels->vels[i].dt)
            if (row == 3) DO_NEW_PARAM(&eqs->vels->vels[i].v)
        }
    }
    for (int i = 0; i < eqs->accs->count; i++) {
        if (row >= 6) {
            row -= 6;
            continue;
        } else {
            if (row == 0) return false;
            if (row == 1) DO_NEW_PARAM(&eqs->accs->accs[i].dx)
            if (row == 2) DO_NEW_PARAM(&eqs->accs->accs[i].dt)
            if (row == 3) DO_NEW_PARAM(&eqs->accs->accs[i].v0)
            if (row == 4) DO_NEW_PARAM(&eqs->accs->accs[i].v)
            if (row == 5) DO_NEW_PARAM(&eqs->accs->accs[i].a)
        }
    }
    return false;
}

static void initVars(Vars *vars) {
    int varNum = 0;
    for (int i = 0; i < CONST_ACC_COUNT; i++) {
        vars->variables[varNum++] = (Variable) {.type = CONSTANT_ACCEL, .eqNum = i, .eqField = 0, .determined = false};
        vars->variables[varNum++] = (Variable) {.type = CONSTANT_ACCEL, .eqNum = i, .eqField = 1, .determined = false};
        vars->variables[varNum++] = (Variable) {.type = CONSTANT_ACCEL, .eqNum = i, .eqField = 2, .determined = false};
        vars->variables[varNum++] = (Variable) {.type = CONSTANT_ACCEL, .eqNum = i, .eqField = 3, .determined = false};
        vars->variables[varNum++] = (Variable) {.type = CONSTANT_ACCEL, .eqNum = i, .eqField = 4, .determined = false};
    }
    for (int i = 0; i < CONST_VEL_COUNT; i++) {
        vars->variables[varNum++] = (Variable) {.type = CONSTANT_VEL, .eqNum = i, .eqField = 0, .determined = false};
        vars->variables[varNum++] = (Variable) {.type = CONSTANT_VEL, .eqNum = i, .eqField = 1, .determined = false};
        vars->variables[varNum++] = (Variable) {.type = CONSTANT_VEL, .eqNum = i, .eqField = 2, .determined = false};
    }
    for (int i = 0; i < VEL_SUM_COUNT; i++) {
        vars->variables[varNum++] = (Variable) {.type = VELOCITY_SUM, .eqNum = i, .eqField = 0, .determined = false};
        vars->variables[varNum++] = (Variable) {.type = VELOCITY_SUM, .eqNum = i, .eqField = 1, .determined = false};
    }
    for (int i = 0; i < FREE_VAR_COUNT; i++) {
        vars->variables[varNum++] = (Variable) {.type = FREE_VAR, .eqNum = i, .eqField = 0, .determined = false};
    }
}

void drawMainMenu(void) {
    static ConstAccs accs = {.count = 0};
    static ConstVels vels = {.count = 0};
    static VelSums velSums = {.count = 0};
    static FreeVars freeVars = {.count = 0};
    static Vars vars;
    initVars(&vars);

    static AllEqs eqs = {
            .accs = &accs,
            .vels = &vels,
            .velSums = &velSums,
            .freeVars = &freeVars
    };
    static MMState state = {
            .eqs = &eqs,
            .selectedRow = 0,
            .scroll = 0,
            .editingParam = NULL
    };

    fixScroll(&state);
    redraw(&state);

    do {
        sk_key_t key = os_GetCSC();
        if (key == sk_Yequ) break;
        if (key == sk_Window || key == sk_Zoom) {
            newEq(&eqs);
            state.scroll = 0;
            redraw(&state);
        }
        if (key == sk_Trace || key == sk_Graph) {
            solve();
            state.scroll = 0;
            redraw(&state);
        }
        if (key == sk_Up && state.selectedRow != 0) {
            state.selectedRow--;
            fixScroll(&state);
            redraw(&state);
        }
        if (key == sk_Down && state.selectedRow != rowCount(&eqs) - 1) {
            state.selectedRow++;
            fixScroll(&state);
            redraw(&state);
        }
        if (key == sk_Enter) {
            if (checkForNewParam(&state)) {}
            redraw(&state);
        }
    } while (true);
}