//
// Created by danie on 12/12/2021.
//

#include "mm_eqs.h"

#include "string.h"

int eqNumForField(EqType eqType, int eqNum, Field field) {
    return varStartOffset[eqType] + eqNum * fieldCounts[eqType] + field;
}

VariableValue *eqForField(AllEqs *eqs, EqType eqType, int eqNum, Field field) {
    return &eqs->variables[eqNumForField(eqType, eqNum, field)];
}

static void fixString(char *str) {
    for (int i = 0; str[i]; i++) {
        if(str[i] == '\x1A') str[i] = '\x0B';
        if(str[i] == '\x1B') str[i] = '\x1C';
    }
}

static void nameForVar(char out[7], AllEqs *eqs, int varNum) {
    EqType type;
    int eqNum;
    Field field;

    if (varNum < eqs->accCount * ACC_FIELD_COUNT) {
        type = ACC;
        eqNum = varNum / ACC_FIELD_COUNT;
        field = varNum % ACC_FIELD_COUNT;
    } else {
        varNum -= eqs->accCount * ACC_FIELD_COUNT;
        if (varNum < eqs->velCount * VEL_FIELD_COUNT) {
            type = VEL;
            eqNum = varNum / VEL_FIELD_COUNT;
            field = varNum % VEL_FIELD_COUNT;
        } else {
            varNum -= eqs->velCount * VEL_FIELD_COUNT;
            if (varNum < eqs->velSumCount * VEL_SUM_FIELD_COUNT) {
                type = VEL_SUM;
                eqNum = varNum / VEL_SUM_FIELD_COUNT;
                field = varNum % VEL_SUM_FIELD_COUNT;
            } else {
                varNum -= eqs->velSumCount * VEL_SUM_FIELD_COUNT;
                type = FREE_VAR;
                eqNum = varNum;
                field = 0;
            }
        }
    }

    if (type == FREE_VAR) {
        strcpy(out, eqs->freeVarNames[eqNum]);
        return;
    }

    if (type == VEL_SUM && field == SUM_V) strcpy(out, "v");
    if (type == VEL_SUM && field == VX) strcpy(out, "vx");
    if (type == VEL_SUM && field == VY) strcpy(out, "vy");

    if (type == VEL && field == VEL_V) strcpy(out, "v");
    if (type == VEL && field == DX) strcpy(out, "\x16x");
    if (type == VEL && field == DT) strcpy(out, "\x16t");

    if (type == ACC && field == DX) strcpy(out, "\x16x");
    if (type == ACC && field == DT) strcpy(out, "\x16t");
    if (type == ACC && field == V0) strcpy(out, "v0");
    if (type == ACC && field == ACC_V) strcpy(out, "v");
    if (type == ACC && field == A) strcpy(out, "a");

    strcat(out, "(");

    if (type == VEL_SUM) strcat(out, eqs->velSumNames[eqNum]);
    if (type == VEL) strcat(out, eqs->velNames[eqNum]);
    if (type == ACC) strcat(out, eqs->accNames[eqNum]);

    strcat(out, ")");
}

void variableDescription(AllEqs *eqs, VariableValue *var, char output[DESCRIPTION_SIZE]) {
    if (var->status == UNDETERMINED) {
        output[0] = '?';
        output[1] = '\0';
    } else if (var->status == CONSTANT) {
        os_RealToStr(output, &var->value, 6, 1, -1);
        fixString(output);
    } else {
        nameForVar(output, eqs, var->eq.varNum);
    }
}