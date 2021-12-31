//
// Created by danie on 12/11/2021.
//

#include "main_menu.h"

#include <tice.h>
#include <graphx.h>
#include <fileioc.h>
#include <solve.h>
#include <string.h>
#include <menu.h>
#include "bottom_menu.h"
#include "new_eq.h"
#include "draw_simple_text.h"
#include "new_variable_value.h"

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
                selectedRow -= state->eqs.velSumCount * VEL_SUM_ROWS;
                if (selectedRow < state->eqs.velCount * VEL_ROWS) {
                    isIndented = selectedRow % VEL_ROWS != 0;
                } else {
                    selectedRow -= state->eqs.velCount * VEL_ROWS;
                    isIndented = selectedRow % ACC_ROWS != 0;
                }
            }
        }
    }

    txt_writeStrAtPos(isIndented ? 1 : 0, screenRow, cursorChar);
}

#define DRAW_ROW(R) do {                  \
    if (screenRow > SCREEN_LINES) break;  \
    if (rowsPassed++ >= state->scroll) {  \
        R;                                \
        screenRow++;                      \
        txt_newline();                    \
    }                                     \
} while (0);

#define DRAW_VALUE(N, T, I, F) DRAW_ROW({                                \
    txt_WriteStrAtCol(1, N);                                             \
    txt_WriteStrAtCol(4, " = ");                                         \
    writeVarDescription(&state->eqs, eqForField(&state->eqs, T, I, F));  \
})


static void drawRows(MMState *state) {
    for (int i = 0; i <= SCREEN_LINES; i++) {
        txt_writeStrAtPos(0, i, EMPTY_ROW);
    }

    int screenRow = 0;
    int rowsPassed = 0;

    txt_setRow(0);
    for (int i = 0; i < state->eqs.freeVarCount; i++) {
        DRAW_VALUE(state->eqs.freeVarNames[i], FREE_VAR, i, VAR)
    }
    for (int i = 0; i < state->eqs.velSumCount; i++) {
        DRAW_VALUE(state->eqs.velSumNames[i], VEL_SUM, i, SUM_V)
        DRAW_VALUE(" vx", VEL_SUM, i, VX)
        DRAW_VALUE(" vy", VEL_SUM, i, VY)
    }
    for (int i = 0; i < state->eqs.velCount; i++) {
        DRAW_ROW(txt_writeStrAtPos(1, screenRow, state->eqs.velNames[i]))
        DRAW_ROW({
                     txt_WriteStrAtCol(1, " \x16");
                     txt_WriteCharAtCol(3, lowercase(state->eqs.velNames[i][0]));
                     txt_WriteStrAtCol(4, " = ");
                     writeVarDescription(&state->eqs, eqForField(&state->eqs, VEL, i, DX));
                 })
        DRAW_VALUE(" \x16t", VEL, i, DT)
        DRAW_VALUE(" v", VEL, i, VEL_V)
    }
    for (int i = 0; i < state->eqs.accCount; i++) {
        DRAW_ROW(txt_writeStrAtPos(1, screenRow, state->eqs.accNames[i]))
        DRAW_ROW({
                     txt_WriteStrAtCol(1, " \x16");
                     txt_WriteCharAtCol(3, lowercase(state->eqs.accNames[i][0]));
                     txt_WriteStrAtCol(4, " = ");
                     writeVarDescription(&state->eqs, eqForField(&state->eqs, ACC, i, DX));
                 })
        DRAW_VALUE(" \x16t", ACC, i, DT)
        DRAW_VALUE(" v0", ACC, i, V0)
        DRAW_VALUE(" v", ACC, i, ACC_V)
        DRAW_VALUE(" a", ACC, i, A)
    }
}

static void drawMenus() {
    bottomMenu(0, 1, "QUIT");
    bottomMenu(1, 2, "NEW COMP");
    bottomMenu(3, 2, "SOLVE");
}

static void fullRedraw(MMState *state) {
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

static bool executeNewVariableValue(MMState *state) {
    int row = state->selectedRow;
    if (row < state->eqs.freeVarCount) {
        newVariableValue(state, eqNumForField(FREE_VAR, row, VAR), false);
        return true;
    }
    row -= state->eqs.freeVarCount;
    if (row < state->eqs.velSumCount * VEL_SUM_ROWS) {
        newVariableValue(state, eqNumForField(VEL_SUM, row / VEL_SUM_ROWS, row % VEL_SUM_ROWS), false);
        return true;
    }
    row -= state->eqs.velSumCount * VEL_SUM_ROWS;
    if (row < state->eqs.velCount * VEL_ROWS) {
        if (row % VEL_ROWS == 0) return false;
        newVariableValue(state, eqNumForField(VEL, row / VEL_ROWS, row % VEL_ROWS - 1), false);
        return true;
    }
    row -= state->eqs.velCount * VEL_ROWS;
    if (row % ACC_ROWS == 0) return false;
    newVariableValue(state, eqNumForField(ACC, row / ACC_ROWS, row % ACC_ROWS - 1), row % ACC_ROWS - 1 == 4);
    return true;
}

static bool deleteValue(MMState * state) {
    EqType type;
    int eqNum;
    Field eqField;

    int row = state->selectedRow;
    if (row < state->eqs.freeVarCount) {
        type = FREE_VAR;
        eqNum = row;
        eqField = VAR;
    } else {
        row -= state->eqs.freeVarCount;
        if (row < state->eqs.velSumCount * VEL_SUM_ROWS) {
            type = VEL_SUM;
            eqNum = row / VEL_SUM_ROWS;
            eqField = row % VEL_SUM_ROWS;
        } else {
            row -= state->eqs.velSumCount * VEL_SUM_ROWS;
            if (row < state->eqs.velCount * VEL_ROWS) {
                if (row % VEL_ROWS == 0) return false;
                type = VEL;
                eqNum = row / VEL_ROWS;
                eqField = row % VEL_ROWS - 1;
            } else {
                row -= state->eqs.velCount * VEL_ROWS;
                if (row % ACC_ROWS == 0) return false;
                type = ACC;
                eqNum = row / ACC_ROWS;
                eqField = row % ACC_ROWS - 1;
            }
        }
    }

    char title[26] = "Delete ";
    if (type == FREE_VAR) {
        strcat(title, state->eqs.freeVarNames[eqNum]);
    } else if (type == VEL_SUM) {
        if (eqField == SUM_V) strcat(title, "v");
        if (eqField == VX) strcat(title, "vx");
        if (eqField == VY) strcat(title, "vy");
        strcat(title, "(");
        strcat(title, state->eqs.velSumNames[eqNum]);
        strcat(title, ")");
    } else if (type == VEL) {
        if (eqField == DX) {
            char str[3] = {'\x16', lowercase(state->eqs.velNames[eqNum][0]), '\0'};
            strcat(title, str);
        }
        if (eqField == DT) strcat(title, "\x16t");
        if (eqField == VEL_V) strcat(title, "v");
        strcat(title, "(");
        strcat(title, state->eqs.velNames[eqNum]);
        strcat(title, ")");
    } else {
        if (eqField == DX) {
            char str[3] = {'\x16', lowercase(state->eqs.accNames[eqNum][0]), '\0'};
            strcat(title, str);
        }
        if (eqField == DT) strcat(title, "\x16t");
        if (eqField == V0) strcat(title, "v0");
        if (eqField == ACC_V) strcat(title, "v");
        if (eqField == A) strcat(title, "a");
        strcat(title, "(");
        strcat(title, state->eqs.accNames[eqNum]);
        strcat(title, ")");
    }
    strcat(title, "'s value?");

    char *options[2];
    options[0] = "No";
    options[1] = "Yes";
    int choice = menu(title, (const char **) options, 2);

    if (choice == 1) {
        VariableValue *var = eqForField(&state->eqs, type, eqNum, eqField);
        var->status.constant = false;
        var->status.variable = false;
        var->status.calculated = false;
    }

    return true;
}

MMState *initMMState(ti_var_t file) {
    AllEqs eqs = {
            .accCount = 0,
            .velCount = 0,
            .velSumCount = 0,
            .freeVarCount = 0,
    };
    MMState state = {
            .eqs = eqs,
            .scroll = 0,
            .selectedRow = 0,
            .editingVar = -1
    };

    MMState *statePtr = ti_GetDataPtr(file);
    ti_Write(&state, sizeof state, 1, file);

    return statePtr;
}

void drawMainMenu(MMState *state) {
    fullRedraw(state);

    do {
        sk_key_t key = os_GetCSC();
        if (key == sk_Yequ) break;
        if (key == sk_Window || key == sk_Zoom) {
            newEq(&state->eqs);
            state->scroll = 0;
            state->selectedRow = 0;
            fullRedraw(state);
        }
        if (key == sk_Trace || key == sk_Graph) {
            solve(&state->eqs);
            state->scroll = 0;
            state->selectedRow = 0;
            fullRedraw(state);
        }
        if (key == sk_Up) doScroll(state, true);
        if (key == sk_Down) doScroll(state, false);

        if (key == sk_Enter) {
            if (executeNewVariableValue(state)) fullRedraw(state);
        }

        if (key == sk_Del) {
            deleteValue(state);
            fullRedraw(state);
        }

        if (key == sk_Clear) {
            clearCalculatedValues(&state->eqs);
            fullRedraw(state);
        }
    } while (true);
}