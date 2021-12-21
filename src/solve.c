//
// Created by danie on 12/20/2021.
//

#include <main_menu/mm_eqs.h>
#include <string.h>
#include "solve.h"
#include "menu.h"

static void setVar(AllEqs *eqs, VariableValue *var, real_t *value) {
    bool hasDependent = var->status == VARIABLE;
    int dependentVar;
    real_t dependentValue;
    if (hasDependent) {
        dependentVar = var->eq.varNum;

        real_t diff = os_RealSub(value, &var->eq.intercept);
        real_t div = os_RealDiv(&diff, &var->eq.coeff);
        dependentValue = div;
    }

    var->status = CONSTANT;
    var->value = *value;

    if (hasDependent) {
        setVar(eqs, &eqs->variables[dependentVar], &dependentValue);
    }
}

static bool updateVars(AllEqs *eqs) {
    bool anyChange = false;
    bool newChange = true;
    while (newChange) {
        newChange = false;
        for (int i = 0; i < VARIABLE_COUNT; i++) {
            VariableValue *var = &eqs->variables[i];
            if (var->status != VARIABLE) continue;
            VariableValue *depVar = &eqs->variables[var->eq.varNum];
            if (depVar->status != CONSTANT) continue;

            real_t depVal = depVar->value;
            real_t product = os_RealMul(&var->eq.coeff, &depVal);
            real_t sum = os_RealAdd(&product, &var->eq.intercept);
            var->status = CONSTANT;
            var->value = sum;

            newChange = true;
            anyChange = true;
        }
    }

    return anyChange;
}

static bool updateVelSums(AllEqs *eqs) {
    bool change = false;
    for (int i = 0; i < eqs->velSumCount; i++) {
        VariableValue *v = eqForField(eqs, VEL_SUM, i, SUM_V);
        VariableValue *vx = eqForField(eqs, VEL_SUM, i, VX);
        VariableValue *vy = eqForField(eqs, VEL_SUM, i, VY);

        if (v->status == CONSTANT && vx->status != CONSTANT && vy->status == CONSTANT) {
            real_t vv = os_RealMul(&v->value, &v->value);
            real_t vyvy = os_RealMul(&vy->value, &vy->value);
            real_t diff = os_RealSub(&vv, &vyvy);
            real_t zero = os_FloatToReal(0.0f);
            if (os_RealCompare(&diff, &zero) >= 0) {
                real_t root = os_RealSqrt(&diff);
                setVar(eqs, vx, &root);
                change = true;
            }
        } else if (v->status == CONSTANT && vx->status == CONSTANT && vy->status != CONSTANT) {
            real_t vv = os_RealMul(&v->value, &v->value);
            real_t vxvx = os_RealMul(&vx->value, &vx->value);
            real_t diff = os_RealSub(&vv, &vxvx);
            real_t zero = os_FloatToReal(0.0f);
            if (os_RealCompare(&diff, &zero) >= 0) {
                real_t root = os_RealSqrt(&diff);
                setVar(eqs, vy, &root);
                change = true;
            }
        } else if (v->status != CONSTANT && vx->status == CONSTANT && vy->status == CONSTANT) {
            real_t vxvx = os_RealMul(&vx->value, &vx->value);
            real_t vyvy = os_RealMul(&vy->value, &vy->value);
            real_t sum = os_RealAdd(&vxvx, &vyvy);
            real_t zero = os_FloatToReal(0.0f);
            if (os_RealCompare(&sum, &zero) >= 0) {
                real_t root = os_RealSqrt(&sum);
                setVar(eqs, v, &root);
                change = true;
            }
        }
    }

    return change;
}

static bool updateConstantVel(AllEqs *eqs) {
    bool change = false;
    for (int i = 0; i < eqs->velCount; i++) {
        VariableValue *v = eqForField(eqs, VEL, i, VEL_V);
        VariableValue *dx = eqForField(eqs, VEL, i, DX);
        VariableValue *dt = eqForField(eqs, VEL, i, DT);

        if (v->status != CONSTANT && dx->status == CONSTANT && dt->status == CONSTANT) {
            real_t div = os_RealDiv(&dx->value, &dt->value);
            setVar(eqs, v, &div);
            change = true;
        } else if (v->status == CONSTANT && dx->status != CONSTANT && dt->status == CONSTANT) {
            real_t product = os_RealMul(&v->value, &dt->value);
            setVar(eqs, dx, &product);
            change = true;
        } else if (v->status == CONSTANT && dx->status == CONSTANT && dt->status != CONSTANT) {
            real_t div = os_RealDiv(&dx->value, &v->value);
            setVar(eqs, dt, &div);
            change = true;
        }
    }

    return change;
}

static bool updateConstantAcc(AllEqs *eqs) {
    bool change = false;
    for (int i = 0; i < eqs->accCount; i++) {
        VariableValue *dx = eqForField(eqs, ACC, i, DX);
        VariableValue *dt = eqForField(eqs, ACC, i, DT);
        VariableValue *v0 = eqForField(eqs, ACC, i, V0);
        VariableValue *v = eqForField(eqs, ACC, i, ACC_V);
        VariableValue *a = eqForField(eqs, ACC, i, A);

        if (dx->status != CONSTANT) {
            if (v0->status == CONSTANT && v->status == CONSTANT && a->status == CONSTANT) {
                real_t vv = os_RealMul(&v->value, &v->value);
                real_t v0v0 = os_RealMul(&v0->value, &v0->value);
                real_t vSquaredMinusV0Squared = os_RealSub(&vv, &v0v0);

                real_t two = os_FloatToReal(2.0f);
                real_t twoA = os_RealMul(&two, &a->value);

                real_t div = os_RealDiv(&vSquaredMinusV0Squared, &twoA);
                setVar(eqs, dx, &div);
                change = true;
            } else if (dt->status == CONSTANT && v->status == CONSTANT && a->status == CONSTANT) {
                real_t vDt = os_RealMul(&v->value, &dt->value);

                real_t half = os_FloatToReal(0.5f);
                real_t halfA = os_RealMul(&half, &a->value);
                real_t dtdt = os_RealMul(&dt->value, &dt->value);
                real_t halfADtSquared = os_RealMul(&halfA, &dtdt);

                real_t diff = os_RealSub(&vDt, &halfADtSquared);
                setVar(eqs, dx, &diff);
                change = true;
            } else if (dt->status == CONSTANT && v0->status == CONSTANT && a->status == CONSTANT) {
                real_t v0dt = os_RealMul(&v0->value, &dt->value);

                real_t half = os_FloatToReal(0.5f);
                real_t halfA = os_RealMul(&half, &a->value);
                real_t dtdt = os_RealMul(&dt->value, &dt->value);
                real_t halfADtSquared = os_RealMul(&halfA, &dtdt);

                real_t sum = os_RealAdd(&v0dt, &halfADtSquared);
                setVar(eqs, dx, &sum);
                change = true;
            } else if (dt->status == CONSTANT && v0->status == CONSTANT && v->status == CONSTANT) {
                real_t half = os_FloatToReal(0.5f);
                real_t v0PlusV = os_RealAdd(&v0->value, &v->value);
                real_t halfV0PlusV = os_RealMul(&half, &v0PlusV);
                real_t product = os_RealMul(&halfV0PlusV, &dt->value);
                setVar(eqs, dx, &product);
                change = true;
            }
        }
        if (dt->status != CONSTANT) {
            if (v0->status == CONSTANT && v->status == CONSTANT && a->status == CONSTANT) {
                real_t diff = os_RealSub(&v->value, &v0->value);
                real_t div = os_RealDiv(&diff, &a->value);
                setVar(eqs, dt, &div);
                change = true;
            } else if (dx->status == CONSTANT && v0->status == CONSTANT && v->status == CONSTANT) {
                real_t two = os_FloatToReal(2.0f);
                real_t twoDx = os_RealMul(&two, &dx->value);
                real_t sum = os_RealAdd(&v->value, &v0->value);
                real_t div = os_RealDiv(&twoDx, &sum);
                setVar(eqs, dt, &div);
                change = true;
            } else if (dx->status == CONSTANT && v->status == CONSTANT && a->status == CONSTANT) {
                real_t vv = os_RealMul(&v->value, &v->value);
                real_t two = os_FloatToReal(2.0f);
                real_t twoA = os_RealMul(&two, &a->value);
                real_t twoADx = os_RealMul(&twoA, &dx->value);
                real_t diff = os_RealSub(&vv, &twoADx);
                real_t zero = os_FloatToReal(0.0f);
                if (os_RealCompare(&diff, &zero) >= 0) {
                    real_t root = os_RealSqrt(&diff);
                    real_t negV = os_RealNeg(&v->value);
                    real_t negA = os_RealNeg(&a->value);

                    real_t plusNum = os_RealAdd(&negV, &root);
                    real_t minusNum = os_RealSub(&negV, &root);
                    real_t plus = os_RealDiv(&plusNum, &negA);
                    real_t minus = os_RealDiv(&minusNum, &negA);

                    char plusStr[7];
                    char minusStr[7];
                    os_RealToStr(plusStr, &plus, 6, 1, -1);
                    os_RealToStr(minusStr, &minus, 6, 1, -1);
                    fixString(plusStr);
                    fixString(minusStr);
                    char *options[2] = {plusStr, minusStr};

                    char title[26] = "Pick a value for \x16t(";
                    strcat(title, eqs->accNames[i]);
                    strcat(title, "):");

                    int choice = menu(title, (const char **) options, 2);
                    if (choice != -1) {
                        setVar(eqs, dt, choice == 0 ? &plus : &minus);
                        change = true;
                    }
                }
            } else if (dx->status == CONSTANT && v0->status == CONSTANT && a->status == CONSTANT) {
                real_t v0v0 = os_RealMul(&v0->value, &v0->value);
                real_t two = os_FloatToReal(2.0f);
                real_t twoA = os_RealMul(&two, &a->value);
                real_t twoADx = os_RealMul(&twoA, &dx->value);
                real_t sum = os_RealAdd(&v0v0, &twoADx);
                real_t zero = os_FloatToReal(0.0f);
                if (os_RealCompare(&sum, &zero) >= 0) {
                    real_t root = os_RealSqrt(&sum);
                    real_t negV0 = os_RealNeg(&v0->value);

                    real_t plusNum = os_RealAdd(&negV0, &root);
                    real_t minusNum = os_RealSub(&negV0, &root);
                    real_t plus = os_RealDiv(&plusNum, &a->value);
                    real_t minus = os_RealDiv(&minusNum, &a->value);

                    char plusStr[7];
                    char minusStr[7];
                    os_RealToStr(plusStr, &plus, 6, 1, -1);
                    os_RealToStr(minusStr, &minus, 6, 1, -1);
                    fixString(plusStr);
                    fixString(minusStr);
                    char *options[2] = {plusStr, minusStr};

                    char title[26] = "Pick a value for \x16t(";
                    strcat(title, eqs->accNames[i]);
                    strcat(title, "):");

                    int choice = menu(title, (const char **) options, 2);
                    if (choice != -1) {
                        setVar(eqs, dt, choice == 0 ? &plus : &minus);
                        change = true;
                    }
                }
            }
        }
        if (v0->status != CONSTANT) {
            if (dt->status == CONSTANT && v->status == CONSTANT && a->status == CONSTANT) {
                real_t aDt = os_RealMul(&a->value, &dt->value);
                real_t diff = os_RealSub(&v->value, &aDt);
                setVar(eqs, v0, &diff);
                change = true;
            } else if (dx->status == CONSTANT && v->status == CONSTANT && a->status == CONSTANT) {
                real_t vv = os_RealMul(&v->value, &v->value);
                real_t two = os_FloatToReal(2.0f);
                real_t twoA = os_RealMul(&two, &a->value);
                real_t twoADx = os_RealMul(&twoA, &dx->value);
                real_t diff = os_RealSub(&vv, &twoADx);
                real_t zero = os_FloatToReal(0.0f);
                if (os_RealCompare(&diff, &zero) >= 0) {
                    real_t root = os_RealSqrt(&diff);
                    setVar(eqs, v0, &root);
                    change = true;
                }
            } else if (dx->status == CONSTANT && dt->status == CONSTANT && a->status == CONSTANT) {
                real_t half = os_FloatToReal(0.5f);
                real_t halfA = os_RealMul(&half, &a->value);
                real_t dtdt = os_RealMul(&dt->value, &dt->value);
                real_t halfADtSquared = os_RealMul(&halfA, &dtdt);
                real_t diff = os_RealSub(&dx->value, &halfADtSquared);
                real_t div = os_RealDiv(&diff, &dt->value);
                setVar(eqs, v0, &div);
                change = true;
            } else if (dx->status == CONSTANT && dt->status == CONSTANT && v->status == CONSTANT) {
                real_t two = os_FloatToReal(2.0f);
                real_t twoDx = os_RealMul(&two, &dx->value);
                real_t div = os_RealDiv(&twoDx, &dt->value);
                real_t diff = os_RealSub(&div, &v->value);
                setVar(eqs, v0, &diff);
                change = true;
            }
        }
        if (v->status != CONSTANT) {
            if (dt->status == CONSTANT && v0->status == CONSTANT && a->status == CONSTANT) {
                real_t aDt = os_RealMul(&a->value, &dt->value);
                real_t sum = os_RealAdd(&v0->value, &aDt);
                setVar(eqs, v, &sum);
                change = true;
            } else if (dx->status == CONSTANT && v0->status == CONSTANT && a->status == CONSTANT) {
                real_t v0v0 = os_RealMul(&v0->value, &v0->value);
                real_t two = os_FloatToReal(2.0f);
                real_t twoA = os_RealMul(&two, &a->value);
                real_t twoADx = os_RealMul(&twoA, &dx->value);
                real_t sum = os_RealAdd(&v0v0, &twoADx);
                real_t zero = os_FloatToReal(0.0f);
                if (os_RealCompare(&sum, &zero) >= 0) {
                    real_t root = os_RealSqrt(&sum);
                    setVar(eqs, v, &root);
                    change = true;
                }
            } else if (dx->status == CONSTANT && v->status == CONSTANT && a->status == CONSTANT) {
                real_t div = os_RealDiv(&dx->value, &dt->value);
                real_t half = os_FloatToReal(0.5f);
                real_t halfA = os_RealMul(&half, &a->value);
                real_t halfADt = os_RealMul(&halfA, &dt->value);
                real_t sum = os_RealAdd(&div, &halfADt);
                setVar(eqs, v, &sum);
                change = true;
            } else if (dx->status == CONSTANT && v0->status == CONSTANT && v->status == CONSTANT) {
                real_t two = os_FloatToReal(2.0f);
                real_t twoDx = os_RealMul(&two, &dx->value);
                real_t div = os_RealMul(&twoDx, &dt->value);
                real_t diff = os_RealSub(&div, &v0->value);
                setVar(eqs, v, &diff);
                change = true;
            }
        }
        if (a->status != CONSTANT) {
            if (dt->status == CONSTANT && v0->status == CONSTANT && v->status == CONSTANT) {
                real_t diff = os_RealSub(&v->value, &v0->value);
                real_t div = os_RealDiv(&diff, &dt->value);
                setVar(eqs, a, &div);
                change = true;
            } else if (dx->status == CONSTANT && v0->status == CONSTANT && v->status == CONSTANT) {
                real_t vv = os_RealMul(&v->value, &v->value);
                real_t v0v0 = os_RealMul(&v0->value, &v0->value);
                real_t diff = os_RealSub(&vv, &v0v0);
                real_t two = os_FloatToReal(2.0f);
                real_t twoDx = os_RealMul(&two, &dx->value);
                real_t div = os_RealDiv(&diff, &twoDx);
                setVar(eqs, a, &div);
                change = true;
            } else if (dx->status == CONSTANT && dt->status == CONSTANT && v->status == CONSTANT) {
                real_t vDt = os_RealMul(&v->value, &dt->value);
                real_t diff = os_RealSub(&dx->value, &vDt);
                real_t dtdt = os_RealMul(&dt->value, &dt->value);
                real_t div = os_RealDiv(&diff, &dtdt);
                real_t negTwo = os_FloatToReal(-2.0f);
                real_t product = os_RealMul(&negTwo, &div);
                setVar(eqs, a, &product);
                change = true;
            } else if (dx->status == CONSTANT && dt->status == CONSTANT && v0->status == CONSTANT) {
                real_t v0Dt = os_RealMul(&v0->value, &dt->value);
                real_t diff = os_RealSub(&dx->value, &v0Dt);
                real_t dtdt = os_RealMul(&dt->value, &dt->value);
                real_t div = os_RealDiv(&diff, &dtdt);
                real_t two = os_FloatToReal(2.0f);
                real_t product = os_RealMul(&two, &div);
                setVar(eqs, a, &product);
                change = true;
            }
        }
    }

    return change;
}

void solve(AllEqs *eqs) {
    bool change = true;
    while (change) {
        change = false;

        change = updateVelSums(eqs);
        change = change || updateConstantVel(eqs);
        change = change || updateConstantAcc(eqs);

        change = change || updateVars(eqs);
    }
}
