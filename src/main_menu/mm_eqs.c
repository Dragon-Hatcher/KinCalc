//
// Created by danie on 12/12/2021.
//

#include "mm_eqs.h"

#include <string.h>
#include "draw_simple_text.h"

int eqNumForField(EqType eqType, int eqNum, Field field) {
    return varStartOffset[eqType] + eqNum * fieldCounts[eqType] + field;
}

VariableValue *eqForField(AllEqs *eqs, EqType eqType, int eqNum, Field field) {
    return &eqs->variables[eqNumForField(eqType, eqNum, field)];
}

VarId varIdForNum(int varNum) {
    if (varNum < ACC_TOT_VAR_COUNT)
        return (VarId) {
                .varNum = varNum,
                .type = ACC,
                .eqNum = varNum / ACC_FIELD_COUNT,
                .field = varNum % ACC_FIELD_COUNT
        };
    varNum -= ACC_TOT_VAR_COUNT;

    if (varNum < VEL_TOT_VAR_COUNT) {
        return (VarId) {
                .varNum = varNum,
                .type = VEL,
                .eqNum = varNum / VEL_FIELD_COUNT,
                .field = varNum % VEL_FIELD_COUNT
        };
    }
    varNum -= VEL_TOT_VAR_COUNT;

    if (varNum < VEL_SUM_TOT_VAR_COUNT) {
        return (VarId) {
                .varNum = varNum,
                .type = VEL_SUM,
                .eqNum = varNum / VEL_SUM_FIELD_COUNT,
                .field = varNum % VEL_SUM_FIELD_COUNT
        };
    }
    varNum -= VEL_SUM_TOT_VAR_COUNT;

    return (VarId) {
            .varNum = varNum,
            .type = FREE_VAR,
            .eqNum = varNum,
            .field = 0
    };
}

void fixString(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] == '\x1A') str[i] = '\x0B';
        if (str[i] == '\x1B') str[i] = '\x1C';
    }
}

static void drawNameForVar(AllEqs *eqs, int varNum) {
    VarId varId = varIdForNum(varNum);
    EqType type = varId.type;
    Field field = varId.field;
    int eqNum = varId.eqNum;

    if (type == FREE_VAR) {
        txt_writeStr(eqs->freeVarNames[eqNum]);
        return;
    }

    if (type == VEL_SUM && field == SUM_V) txt_writeStr("v");
    if (type == VEL_SUM && field == VX) txt_writeStr("vx");
    if (type == VEL_SUM && field == VY) txt_writeStr("vy");

    if (type == VEL && field == VEL_V) txt_writeStr("v");
    if (type == VEL && field == DX) {
        txt_writeStr("\x16");
        txt_WriteChar(lowercase(eqs->velNames[eqNum][0]));
    }
    if (type == VEL && field == DT) txt_writeStr("\x16t");

    if (type == ACC && field == DX) {
        txt_writeStr("\x16");
        txt_WriteChar(lowercase(eqs->accNames[eqNum][0]));
    }
    if (type == ACC && field == DT) txt_writeStr("\x16t");
    if (type == ACC && field == V0) txt_writeStr("v0");
    if (type == ACC && field == ACC_V) txt_writeStr("v");
    if (type == ACC && field == A) txt_writeStr("a");

    txt_writeStr("(");

    if (type == VEL_SUM) txt_writeStr(eqs->velSumNames[eqNum]);
    if (type == VEL) txt_writeStr(eqs->velNames[eqNum]);
    if (type == ACC) txt_writeStr(eqs->accNames[eqNum]);

    txt_writeStr(")");
}


void writeVarDescription(AllEqs *eqs, VariableValue *var) {
    if (var->status == UNDETERMINED) {
        txt_writeStr("?");
    } else if (var->status == CONSTANT) {
        char out[7];
        os_RealToStr(out, &var->value, 6, 1, -1);
        fixString(out);
        txt_writeStr(out);
    } else {
        real_t zero = os_FloatToReal(0.0f);
        real_t one = os_FloatToReal(1.0f);

        if (os_RealCompare(&one, &var->eq.coeff) != 0) {
            char out[7];
            os_RealToStr(out, &var->eq.coeff, 6, 1, -1);
            fixString(out);
            txt_writeStr(out);
        }

        drawNameForVar(eqs, var->eq.varNum);

        if (os_RealCompare(&zero, &var->eq.intercept) != 0) {
            real_t intercept = var->eq.intercept;
            bool isNeg = os_RealCompare(&zero, &intercept) == 1;
            if (isNeg) intercept = os_RealNeg(&intercept);

            txt_writeStr(isNeg ? "-" : "+");

            char out[7];
            os_RealToStr(out, &intercept, 6, 1, -1);
            fixString(out);
            txt_writeStr(out);
        }
    }
}