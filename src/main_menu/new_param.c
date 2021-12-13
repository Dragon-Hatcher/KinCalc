//
// Created by danie on 12/12/2021.
//

#include "new_param.h"

#include <string.h>
#include "menu.h"

static Variable *chooseVar(AllEqs *eqs, Variable *except) {
    int varCount = eqs->accs->count * 5 + eqs->vels->count * 3 + eqs->velSums->count * 2 + eqs->freeVars->count;
    int varNum = 0;
    char strs[varCount][NAME_SIZE + 3];
    for (int i = 0; i < varCount; i++) {
        for (int j = 0; j < NAME_SIZE + 3; j++) {
            strs[i][j] = '\0';
        }
    }

    for (int i = 0; i < eqs->freeVars->count; i++) {
        FreeVar var = eqs->freeVars->vars[i];
        strcat(strs[varNum++], var.name);
    }
    for (int i = 0; i < eqs->velSums->count; i++) {
        VelSum velSum = eqs->velSums->sums[i];
        strcat(strs[varNum], velSum.name);
        strcat(strs[varNum],strlen(velSum.name) == 1 ? ": " : ":");
        strcat(strs[varNum++], ":vx");
        strcat(strs[varNum++], "   vy");
    }
    for (int i = 0; i < eqs->vels->count; i++) {
        ConstVel vel = eqs->vels->vels[i];
        strcat(strs[varNum], vel.name);
        strcat(strs[varNum],strlen(vel.name) == 1 ? ": " : ":");
        strcat(strs[varNum++], ":\x16x");
        strcat(strs[varNum++], "   \x16t");
        strcat(strs[varNum++], "   v ");
    }
    for (int i = 0; i < eqs->accs->count; i++) {
        ConstAcc acc = eqs->accs->accs[i];
        strcat(strs[varNum], acc.name);
        strcat(strs[varNum],strlen(acc.name) == 1 ? ": " : ":");
        strcat(strs[varNum++], "\x16x");
        strcat(strs[varNum++], "   \x16t");
        strcat(strs[varNum++], "   v0");
        strcat(strs[varNum++], "   v ");
        strcat(strs[varNum++], "   a ");
    }
    char *options[varCount];
    for (int i = 0; i < varCount; i++) {
        options[i] = strs[i];
    }

    int choice = menu("Choose a Variable:", (const char **) options, varCount);
    if (choice == -1) return NULL;

    for (int i = 0; i < eqs->freeVars->count; i++) {
        if (choice-- == 0) return varForTypeAndNum(eqs, FREE_VAR, i, 0);
    }
    for (int i = 0; i < eqs->velSums->count; i++) {
        if (choice-- == 0) return varForTypeAndNum(eqs, VELOCITY_SUM, i, 0);
        if (choice-- == 0) return varForTypeAndNum(eqs, VELOCITY_SUM, i, 1);
    }
    for (int i = 0; i < eqs->vels->count; i++) {
        if (choice-- == 0) return varForTypeAndNum(eqs, CONSTANT_VEL, i, 0);
        if (choice-- == 0) return varForTypeAndNum(eqs, CONSTANT_VEL, i, 1);
        if (choice-- == 0) return varForTypeAndNum(eqs, CONSTANT_VEL, i, 2);
    }
    for (int i = 0; i < eqs->accs->count; i++) {
        if (choice-- == 0) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 0);
        if (choice-- == 0) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 1);
        if (choice-- == 0) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 2);
        if (choice-- == 0) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 3);
        if (choice-- == 0) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 4);
    }
    return NULL;
}

void newParam(Param *param, MMState *state) {
    char *eqs[5];
    eqs[0] = "b";
    eqs[1] = "x";
    eqs[2] = "ax";
    eqs[3] = "x + b";
    eqs[4] = "ax + b";

    int choiceNum = menu("Value Type:", (const char **) eqs, 5);
    if (choiceNum == -1) return;

    ParamType choice = (ParamType)choiceNum;
    state->editingParam = param;
    state->editingParamType = choice;

    Variable *defVar = varForParam(state->eqs, param);

    param->type = choice;

    if (choice == CONSTANT) {
        param->var = defVar;
    } else {
        Variable *var = chooseVar(state->eqs, defVar);
        if (var == NULL) return;
        param->var = var;
    }

    if (choice == CONSTANT) param->coeff = os_FloatToReal(0.0f);
    if (choice == VAR || choice == OFFSET) param->coeff = os_FloatToReal(1.0f);

    if (choice == VAR || choice == COEFF) param->offset = os_FloatToReal(0.0f);
}