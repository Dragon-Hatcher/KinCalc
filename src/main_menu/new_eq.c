//
// Created by danie on 12/12/2021.
//

#include "new_eq.h"

#include <graphx.h>
#include <textInput.h>
#include <string.h>
#include "menu.h"
#include "draw_simple_text.h"

static VariableValue EMPTY_EQ = (VariableValue) {
    .status = UNDETERMINED
};

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
    if (eqs->accCount == ACC_VAR_COUNT) return;
    strcpy(eqs->accNames[eqs->accCount], name);

    *eqForField(eqs, ACC, eqs->accCount, DX) = EMPTY_EQ;
    *eqForField(eqs, ACC, eqs->accCount, DT) = EMPTY_EQ;
    *eqForField(eqs, ACC, eqs->accCount, V0) = EMPTY_EQ;
    *eqForField(eqs, ACC, eqs->accCount, ACC_V) = EMPTY_EQ;
    *eqForField(eqs, ACC, eqs->accCount, A) = EMPTY_EQ;

    eqs->accCount++;
}

static void addConstVel(AllEqs *eqs, const char name[NAME_SIZE]) {
    if (eqs->velCount == VEL_VAR_COUNT) return;
    strcpy(eqs->velNames[eqs->velCount], name);

    *eqForField(eqs, VEL, eqs->velCount, DX) = EMPTY_EQ;
    *eqForField(eqs, VEL, eqs->velCount, DT) = EMPTY_EQ;
    *eqForField(eqs, VEL, eqs->velCount, VEL_V) = EMPTY_EQ;

    eqs->velCount++;
}

static void addVelSum(AllEqs *eqs, const char name[NAME_SIZE]) {
    if (eqs->velSumCount == VEL_SUM_VAR_COUNT) return;
    strcpy(eqs->velSumNames[eqs->velSumCount], name);

    *eqForField(eqs, VEL_SUM, eqs->velSumCount, SUM_V) = EMPTY_EQ;
    *eqForField(eqs, VEL_SUM, eqs->velSumCount, VX) = EMPTY_EQ;
    *eqForField(eqs, VEL_SUM, eqs->velSumCount, VY) = EMPTY_EQ;

    eqs->velSumCount++;
}

static void addFreeVar(AllEqs *eqs, const char name[NAME_SIZE]) {
    if (eqs->freeVarCount == FREE_VAR_VAR_COUNT) return;
    strcpy(eqs->freeVarNames[eqs->freeVarCount], name);

    *eqForField(eqs, FREE_VAR, eqs->freeVarCount, VAR) = EMPTY_EQ;

    eqs->freeVarCount++;
}

void newEq(AllEqs *eqs) {
    int choiceNum = newEqType();
    if (choiceNum == -1) return;
    EqType choice = (EqType) choiceNum;

    char name[NAME_SIZE];
    getName(name);

    switch (choice) {
        case ACC:
            addConstAcc(eqs, name);
            break;
        case VEL:
            addConstVel(eqs, name);
            break;
        case VEL_SUM:
            addVelSum(eqs, name);
            break;
        case FREE_VAR:
            addFreeVar(eqs, name);
            break;
        default:
            break;
    }
}
