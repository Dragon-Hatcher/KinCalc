//
// Created by danie on 12/12/2021.
//

#include "new_eq.h"

#include <graphx.h>
#include <textInput.h>
#include <string.h>
#include "menu.h"
#include "draw_simple_text.h"
#include "mm_eqs.h"

static int newEqType(void) {
    char *eqTypes[4];
    eqTypes[0] = "Constant Acceleration";
    eqTypes[1] = "Constant Velocity";
    eqTypes[2] = "Velocity from Components";
    eqTypes[3] = "Free Variable";
    return menu("Equation Type:", (const char **) eqTypes, 4);
}

static void getName(char nameOut[NAME_SIZE]) {
    gfx_FillScreen(0xFF);
    drawAtCharPos(0, 0, "Name: ");
    textInput(nameOut, NAME_CHAR_COUNT, 1, 0, 5);
}

static void addConstAcc(AllEqs *eqs, const char name[NAME_SIZE]) {
    if (eqs->accs->count == CONST_ACC_COUNT) return;
    ConstAcc newAcc = {
            .dx = emptyParamValue(),
            .dt = emptyParamValue(),
            .v0 = emptyParamValue(),
            .v = emptyParamValue(),
            .a = emptyParamValue()
    };
    strcpy(newAcc.name, name);
    eqs->accs->accs[eqs->accs->count] = newAcc;
    eqs->accs->count++;
}

static void addConstVel(AllEqs *eqs, const char name[NAME_SIZE]) {
    if (eqs->vels->count == CONST_VEL_COUNT) return;
    ConstVel newVel = {
            .dx = emptyParamValue(),
            .dt = emptyParamValue(),
            .v = emptyParamValue()
    };
    strcpy(newVel.name, name);
    eqs->vels->vels[eqs->vels->count] = newVel;
    eqs->vels->count++;
}

static void addVelSum(AllEqs *eqs, const char name[NAME_SIZE]) {
    if (eqs->velSums->count == VEL_SUM_COUNT) return;
    VelSum newSum = {
            .x = emptyParamValue(),
            .y = emptyParamValue()
    };
    strcpy(newSum.name, name);
    eqs->velSums->sums[eqs->velSums->count] = newSum;
    eqs->velSums->count++;
}

static void addFreeVar(AllEqs *eqs, const char name[NAME_SIZE]) {
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

    char name[NAME_SIZE];
    getName(name);

    switch (choice) {
        case CONSTANT_ACCEL:
            addConstAcc(eqs, name);
            break;
        case CONSTANT_VEL:
            addConstVel(eqs, name);
            break;
        case VELOCITY_SUM:
            addVelSum(eqs, name);
            break;
        case FREE_VAR:
            addFreeVar(eqs, name);
            break;
        default:
            break;
    }
}
