//
// Created by danie on 12/12/2021.
//

#include "mm_eqs.h"

int eqNumForField(AllEqs *eqs, EqType eqType, int eqNum, Field field) {
    return varStartOffset[eqType] + eqNum * fieldCounts[eqType] + field;
}

VariableValue *eqForField(AllEqs *eqs, EqType eqType, int eqNum, Field field) {
    return &eqs->variables[eqNumForField(eqs, eqType, eqNum, field)];
}

void variableDescription(AllEqs *eqs, VariableValue *var, char output[DESCRIPTION_SIZE]) {
    if (var->status == UNDETERMINED) {
        output[0] = '?';
        output[1] = '\0';
    } else if (var->status == CONSTANT) {
        os_RealToStr(output, &var->value, DESCRIPTION_CHARS, 1, 3);
    } else {
        output[0] = 'v';
        output[1] = '\0';
    }
}