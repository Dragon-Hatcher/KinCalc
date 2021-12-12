//
// Created by danie on 12/11/2021.
//

#include "var_set.h"

#include <tice.h>

bool isVarSetResolved(VarSet *varSet) {
    return
            isLinearEqResolved(&varSet->dx)
            && isLinearEqResolved(&varSet->dt)
            && isLinearEqResolved(&varSet->v0)
            && isLinearEqResolved(&varSet->v)
            && isLinearEqResolved(&varSet->a);
}

//Linear Equations -----------------------------------------------------------------------------------------------------

bool isLinearEqResolved(LinearEq *eq) {
    return eq->variable->resolved;
}

real_t getValueOfLinearEq(LinearEq *eq) {
    real_t mul = os_RealMul(&eq->coeff, &eq->variable->value);
    return os_RealAdd(&mul, &eq->intercept);
}

void setValueOfLinearEq(LinearEq *eq, real_t value) {
    real_t sub = os_RealSub(&value, &eq->intercept);
    real_t newValue = os_RealDiv(&sub, &eq->coeff);
    eq->variable->value = newValue;
}