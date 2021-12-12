//
// Created by danie on 12/12/2021.
//

#include "new_eq.h"

#include <graphx.h>
#include <textInput.h>
#include <string.h>
#include "menu.h"
#include "draw_simple_text.h"

static int newEqType(void) {
    char *eqTypes[4];
    eqTypes[0] = "Constant Acceleration";
    eqTypes[1] = "Constant Velocity";
    eqTypes[2] = "Velocity from Components";
    eqTypes[3] = "Free Variable";
    return menu("Equation Type:", (const char **) eqTypes, 4);
}

static void getName(char nameOut[NAME_LENGTH + 1]) {
    gfx_FillScreen(0xFF);
    drawAtCharPos(0, 0, "Name: ");
    textInput(nameOut, NAME_LENGTH, 1, 0, 5);
}

static void addFreeVar(AllEqs *eqs, const char name[NAME_LENGTH + 1]) {
    if (eqs->freeVars->count == FREE_VAR_COUNT) return;
    FreeVar newVar;
    strcpy(newVar.name, name);
    eqs->freeVars->vars[eqs->freeVars->count] = newVar;
    eqs->freeVars->count++;
}

void newEq(AllEqs *eqs) {
    int choiceNum = newEqType();
    if (choiceNum == -1) return;
    EqType choice = (EqType) choiceNum;

    char name[NAME_LENGTH + 1];
    getName(name);

    switch (choice) {
        case CONSTANT_ACCEL:
            break;
        case CONSTANT_VEL:
            break;
        case VELOCITY_SUM:
            break;
        case FREE_VAR:
            addFreeVar(eqs, name);
            break;
        default:
            break;
    }
}
