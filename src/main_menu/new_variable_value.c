//
// Created by danie on 12/12/2021.
//

#include "new_variable_value.h"

#include <string.h>
#include <draw_simple_text.h>
#include "menu.h"
#include "call_ti_basic.h"

static int chooseVar(AllEqs *eqs, int exceptVarNum) {
    int varCount = eqs->accCount * 5 + eqs->velCount * 3 + eqs->velSumCount * 3 + eqs->freeVarCount;
    int varNum = 0;
    char strs[varCount][NAME_SIZE + 3];
    int assoc[varCount];
    for (int i = 0; i < varCount; i++) {
        for (int j = 0; j < NAME_SIZE + 3; j++) {
            strs[i][j] = '\0';
        }
    }

    for (int i = 0; i < eqs->freeVarCount; i++) {
        strcat(strs[varNum], eqs->freeVarNames[i]);
        assoc[varNum] = eqNumForField(FREE_VAR, i, VAR);
        strcat(strs[varNum++], "   ");
    }
    for (int i = 0; i < eqs->velSumCount; i++) {
        assoc[varNum] = eqNumForField(VEL_SUM, i, SUM_V);
        strcat(strs[varNum], eqs->velSumNames[i]);
        strcat(strs[varNum++], "   ");
        assoc[varNum] = eqNumForField(VEL_SUM, i, VX);
        strcat(strs[varNum++], "   vx");
        assoc[varNum] = eqNumForField(VEL_SUM, i, VY);
        strcat(strs[varNum++], "   vy");
    }
    for (int i = 0; i < eqs->velCount; i++) {
        assoc[varNum] = eqNumForField(VEL, i, DX);
        strcat(strs[varNum], eqs->velNames[i]);
        strcat(strs[varNum], strlen(eqs->velNames[i]) == 1 ? ": \x16" : ":\x16");
        char str[2] = {lowercase(eqs->velNames[i][0]), '\0'};
        strcat(strs[varNum++], str);
        assoc[varNum] = eqNumForField(VEL, i, DT);
        strcat(strs[varNum++], "   \x16t");
        assoc[varNum] = eqNumForField(VEL, i, VEL_V);
        strcat(strs[varNum++], "   v ");
    }
    for (int i = 0; i < eqs->accCount; i++) {
        strcat(strs[varNum], eqs->accNames[i]);
        strcat(strs[varNum], strlen(eqs->accNames[i]) == 1 ? ": \x16" : ":\x16");
        assoc[varNum] = eqNumForField(ACC, i, DX);
        char str[2] = {lowercase(eqs->accNames[i][0]), '\0'};
        strcat(strs[varNum++], str);
        assoc[varNum] = eqNumForField(ACC, i, DT);
        strcat(strs[varNum++], "   \x16t");
        assoc[varNum] = eqNumForField(ACC, i, V0);
        strcat(strs[varNum++], "   v0");
        assoc[varNum] = eqNumForField(ACC, i, ACC_V);
        strcat(strs[varNum++], "   v ");
        assoc[varNum] = eqNumForField(ACC, i, A);
        strcat(strs[varNum++], "   a ");
    }
    char *options[varCount];
    for (int i = 0; i < varCount; i++) {
        options[i] = strs[i];
    }

    int except;
    VarId exceptVarId = varIdForNum(exceptVarNum);
    if (exceptVarId.type == FREE_VAR) {
        except = exceptVarId.eqNum;
    } else if (exceptVarId.type == VEL_SUM) {
        except = eqs->freeVarCount + (exceptVarId.eqNum * VEL_SUM_FIELD_COUNT) + exceptVarId.field;
    } else if (exceptVarId.type == VEL) {
        except = eqs->freeVarCount + (eqs->velSumCount * VEL_SUM_FIELD_COUNT) + (exceptVarId.eqNum * VEL_FIELD_COUNT) +
                 exceptVarId.field;
    } else {
        except = eqs->freeVarCount + (eqs->velSumCount * VEL_SUM_FIELD_COUNT) + (eqs->velCount * VEL_FIELD_COUNT) +
                 (exceptVarId.eqNum * ACC_FIELD_COUNT) + exceptVarId.field;
    }

    int choice = menuExcept("Choose a Variable:", (const char **) options, varCount, except);
    if (choice == -1) return -1;
    return assoc[choice];
}

void newVariableValue(MMState *state, int varNumber) {
    char *eqOptions[5];
    eqOptions[0] = "b";
    eqOptions[1] = "x";
    eqOptions[2] = "ax";
    eqOptions[3] = "x + b";
    eqOptions[4] = "ax + b";

    int choiceNum = menu("Value Type:", (const char **) eqOptions, 5);
    if (choiceNum == -1) return;

    VariableValue *var = &state->eqs.variables[varNumber];
    state->editingVar = varNumber;

    if (choiceNum == 0) {
        var->status.constant = true;
        var->status.calculated = false;
        getVariableInput(state, VAR_B);
    } else {
        int termVar = chooseVar(&state->eqs, varNumber);
        if (termVar == -1) return;

        var->status.variable = true;
        LinearEq *varEq = &var->eq;
        varEq->varNum = termVar;

        if (choiceNum == 1 || choiceNum == 2) varEq->intercept = os_FloatToReal(0.0f);
        if (choiceNum == 1 || choiceNum == 3) varEq->coeff = os_FloatToReal(1.0f);

        if (choiceNum == 2) getVariableInput(state, VAR_A);
        if (choiceNum == 3) getVariableInput(state, VAR_B);
        if (choiceNum == 4) getVariableInput(state, VARS_A_AND_B);
    }
}