//
// Created by danie on 12/20/2021.
//

#include <main_menu/mm_eqs.h>
#include <string.h>
#include <graphx.h>
#include "solve.h"
#include "menu.h"
#include "draw_simple_text.h"

IllegalConfig illegalConfig = {
        .type = NONE
};

static void setVar(AllEqs *eqs, VariableValue *var, real_t *value) {
    if (var->status.constant) return;

    bool hasDependent = var->status.variable;
    int dependentVar;
    real_t dependentValue;
    if (hasDependent) {
        dependentVar = var->eq.varNum;

        real_t diff = os_RealSub(value, &var->eq.intercept);
        real_t div = os_RealDiv(&diff, &var->eq.coeff);
        dependentValue = div;
    }

    var->status.calculated = true;
    var->status.constant = true;
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
            if (var->status.constant || !var->status.variable) continue;
            VariableValue *depVar = &eqs->variables[var->eq.varNum];
            if (!depVar->status.constant) continue;

            real_t depVal = depVar->value;
            real_t product = os_RealMul(&var->eq.coeff, &depVal);
            real_t sum = os_RealAdd(&product, &var->eq.intercept);
            var->status.calculated = true;
            var->status.constant = true;
            var->value = sum;

            newChange = true;
            anyChange = true;
        }
    }

    return anyChange;
}

#define CHECK_DIV_BY_ZERO(VAL, TYPE) do {     \
    if (os_RealCompare(&(VAL), &zero) == 0) { \
        illegalConfig = (IllegalConfig) {     \
            .type = (TYPE),                   \
            .eqNum = i                        \
        };                                    \
        return  change;                       \
    }                                         \
} while(0);

#define CHECK_NEG_ROOT(VAL, TYPE) do {       \
    if (os_RealCompare(&(VAL), &zero) < 0) { \
        illegalConfig = (IllegalConfig) {    \
            .type = (TYPE),                  \
            .eqNum = i                       \
        };                                   \
        return  change;                      \
    }                                        \
} while(0);

static bool updateVelSums(AllEqs *eqs) {
    real_t zero = os_FloatToReal(0.0f);

    bool change = false;
    for (int i = 0; i < eqs->velSumCount; i++) {
        VariableValue *v = eqForField(eqs, VEL_SUM, i, SUM_V);
        VariableValue *vx = eqForField(eqs, VEL_SUM, i, VX);
        VariableValue *vy = eqForField(eqs, VEL_SUM, i, VY);

        if (v->status.constant && !vx->status.constant && vy->status.constant) {
            real_t vv = os_RealMul(&v->value, &v->value);
            real_t vyvy = os_RealMul(&vy->value, &vy->value);
            real_t diff = os_RealSub(&vv, &vyvy);
            CHECK_NEG_ROOT(diff, VY_GREATER_THAN_V)
            real_t root = os_RealSqrt(&diff);
            setVar(eqs, vx, &root);
            change = true;
        } else if (v->status.constant && vx->status.constant && !vy->status.constant) {
            real_t vv = os_RealMul(&v->value, &v->value);
            real_t vxvx = os_RealMul(&vx->value, &vx->value);
            real_t diff = os_RealSub(&vv, &vxvx);
            CHECK_NEG_ROOT(diff, VX_GREATER_THAN_V)
            real_t root = os_RealSqrt(&diff);
            setVar(eqs, vy, &root);
            change = true;
        } else if (!v->status.constant && vx->status.constant && vy->status.constant) {
            real_t vxvx = os_RealMul(&vx->value, &vx->value);
            real_t vyvy = os_RealMul(&vy->value, &vy->value);
            real_t sum = os_RealAdd(&vxvx, &vyvy);
            real_t root = os_RealSqrt(&sum);
            setVar(eqs, v, &root);
            change = true;
        }
    }

    return change;
}

static bool updateConstantVel(AllEqs *eqs) {
    real_t zero = os_FloatToReal(0.0f);

    bool change = false;
    for (int i = 0; i < eqs->velCount; i++) {
        VariableValue *v = eqForField(eqs, VEL, i, VEL_V);
        VariableValue *dx = eqForField(eqs, VEL, i, DX);
        VariableValue *dt = eqForField(eqs, VEL, i, DT);

        if (!v->status.constant && dx->status.constant && dt->status.constant) {
            CHECK_DIV_BY_ZERO(dt->value, VEL_T_ZERO)
            real_t div = os_RealDiv(&dx->value, &dt->value);
            setVar(eqs, v, &div);
            change = true;
        } else if (v->status.constant && !dx->status.constant && dt->status.constant) {
            real_t product = os_RealMul(&v->value, &dt->value);
            setVar(eqs, dx, &product);
            change = true;
        } else if (v->status.constant && dx->status.constant && !dt->status.constant) {
            CHECK_DIV_BY_ZERO(v->value, VEL_ZERO)
            real_t div = os_RealDiv(&dx->value, &v->value);
            setVar(eqs, dt, &div);
            change = true;
        }
    }

    return change;
}

static bool updateConstantAcc(AllEqs *eqs) {
    real_t zero = os_FloatToReal(0.0f);

    bool change = false;
    for (int i = 0; i < eqs->accCount; i++) {
        VariableValue *dx = eqForField(eqs, ACC, i, DX);
        VariableValue *dt = eqForField(eqs, ACC, i, DT);
        VariableValue *v0 = eqForField(eqs, ACC, i, V0);
        VariableValue *v = eqForField(eqs, ACC, i, ACC_V);
        VariableValue *a = eqForField(eqs, ACC, i, A);

        if (!dx->status.constant) {
            if (v0->status.constant && v->status.constant && a->status.constant) {
                CHECK_DIV_BY_ZERO(a->value, ACC_ZERO)

                real_t vv = os_RealMul(&v->value, &v->value);
                real_t v0v0 = os_RealMul(&v0->value, &v0->value);
                real_t vSquaredMinusV0Squared = os_RealSub(&vv, &v0v0);

                real_t two = os_FloatToReal(2.0f);
                real_t twoA = os_RealMul(&two, &a->value);

                real_t div = os_RealDiv(&vSquaredMinusV0Squared, &twoA);
                setVar(eqs, dx, &div);
                change = true;
            } else if (dt->status.constant && v->status.constant && a->status.constant) {
                real_t vDt = os_RealMul(&v->value, &dt->value);

                real_t half = os_FloatToReal(0.5f);
                real_t halfA = os_RealMul(&half, &a->value);
                real_t dtdt = os_RealMul(&dt->value, &dt->value);
                real_t halfADtSquared = os_RealMul(&halfA, &dtdt);

                real_t diff = os_RealSub(&vDt, &halfADtSquared);
                setVar(eqs, dx, &diff);
                change = true;
            } else if (dt->status.constant && v0->status.constant && a->status.constant) {
                real_t v0dt = os_RealMul(&v0->value, &dt->value);

                real_t half = os_FloatToReal(0.5f);
                real_t halfA = os_RealMul(&half, &a->value);
                real_t dtdt = os_RealMul(&dt->value, &dt->value);
                real_t halfADtSquared = os_RealMul(&halfA, &dtdt);

                real_t sum = os_RealAdd(&v0dt, &halfADtSquared);
                setVar(eqs, dx, &sum);
                change = true;
            } else if (dt->status.constant && v0->status.constant && v->status.constant) {
                real_t half = os_FloatToReal(0.5f);
                real_t v0PlusV = os_RealAdd(&v0->value, &v->value);
                real_t halfV0PlusV = os_RealMul(&half, &v0PlusV);
                real_t product = os_RealMul(&halfV0PlusV, &dt->value);
                setVar(eqs, dx, &product);
                change = true;
            }
        }
        if (!dt->status.constant) {
            if (v0->status.constant && v->status.constant && a->status.constant) {
                CHECK_DIV_BY_ZERO(a->value, ACC_ZERO)

                real_t diff = os_RealSub(&v->value, &v0->value);
                real_t div = os_RealDiv(&diff, &a->value);
                setVar(eqs, dt, &div);
                change = true;
            } else if (dx->status.constant && v0->status.constant && v->status.constant) {
                real_t sum = os_RealAdd(&v->value, &v0->value);
                CHECK_DIV_BY_ZERO(sum, ACC_VEL_SUM)
                real_t two = os_FloatToReal(2.0f);
                real_t twoDx = os_RealMul(&two, &dx->value);
                real_t div = os_RealDiv(&twoDx, &sum);
                setVar(eqs, dt, &div);
                change = true;
            } else if (dx->status.constant && v->status.constant && a->status.constant) {
                real_t vv = os_RealMul(&v->value, &v->value);
                real_t two = os_FloatToReal(2.0f);
                real_t twoA = os_RealMul(&two, &a->value);
                real_t twoADx = os_RealMul(&twoA, &dx->value);
                real_t diff = os_RealSub(&vv, &twoADx);
                CHECK_NEG_ROOT(diff, ACC_IMPOSSIBLE_FINAL_V)
                CHECK_DIV_BY_ZERO(a->value, ACC_ZERO)

                real_t root = os_RealSqrt(&diff);
                real_t negV = os_RealNeg(&v->value);
                real_t negA = os_RealNeg(&a->value);

                real_t plusNum = os_RealAdd(&negV, &root);
                real_t minusNum = os_RealSub(&negV, &root);
                real_t plus = os_RealDiv(&plusNum, &negA);
                real_t minus = os_RealDiv(&minusNum, &negA);

                bool plusIsPos = os_RealCompare(&plus, &zero) >= 0;
                bool minusIsPos = os_RealCompare(&minus, &zero) >= 0;

                bool usingPlus;
                bool possibleValue = true;

                if (plusIsPos && !minusIsPos) {
                    usingPlus = true;
                } else if (!plusIsPos && minusIsPos) {
                    usingPlus = false;
                } else if (plusIsPos && minusIsPos) {
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
                    usingPlus = choice == 0;
                    possibleValue = choice != -1;
                } else {
                    possibleValue = false;
                }
                if (possibleValue) {
                    setVar(eqs, dt, usingPlus ? &plus : &minus);
                    change = true;
                }
            } else if (dx->status.constant && v0->status.constant && a->status.constant) {
                real_t v0v0 = os_RealMul(&v0->value, &v0->value);
                real_t two = os_FloatToReal(2.0f);
                real_t twoA = os_RealMul(&two, &a->value);
                real_t twoADx = os_RealMul(&twoA, &dx->value);
                real_t sum = os_RealAdd(&v0v0, &twoADx);
                CHECK_NEG_ROOT(sum, ACC_IMPOSSIBLE_INITIAL_V)
                CHECK_DIV_BY_ZERO(a->value, ACC_ZERO)

                real_t root = os_RealSqrt(&sum);
                real_t negV0 = os_RealNeg(&v0->value);

                real_t plusNum = os_RealAdd(&negV0, &root);
                real_t minusNum = os_RealSub(&negV0, &root);
                real_t plus = os_RealDiv(&plusNum, &a->value);
                real_t minus = os_RealDiv(&minusNum, &a->value);

                bool plusIsPos = os_RealCompare(&plus, &zero) >= 0;
                bool minusIsPos = os_RealCompare(&minus, &zero) >= 0;

                bool usingPlus;
                bool possibleValue = true;

                if (plusIsPos && !minusIsPos) {
                    usingPlus = true;
                } else if (!plusIsPos && minusIsPos) {
                    usingPlus = false;
                } else if (plusIsPos && minusIsPos) {
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
                    usingPlus = choice == 0;
                    possibleValue = choice != -1;
                } else {
                    possibleValue = false;
                }
                if (possibleValue) {
                    setVar(eqs, dt, usingPlus ? &plus : &minus);
                    change = true;
                }
            }
        }
        if (!v0->status.constant) {
            if (dt->status.constant && v->status.constant && a->status.constant) {
                real_t aDt = os_RealMul(&a->value, &dt->value);
                real_t diff = os_RealSub(&v->value, &aDt);
                setVar(eqs, v0, &diff);
                change = true;
            } else if (dx->status.constant && v->status.constant && a->status.constant) {
                real_t vv = os_RealMul(&v->value, &v->value);
                real_t two = os_FloatToReal(2.0f);
                real_t twoA = os_RealMul(&two, &a->value);
                real_t twoADx = os_RealMul(&twoA, &dx->value);
                real_t diff = os_RealSub(&vv, &twoADx);
                CHECK_NEG_ROOT(diff, ACC_IMPOSSIBLE_FINAL_V)
                real_t root = os_RealSqrt(&diff);
                setVar(eqs, v0, &root);
                change = true;
            } else if (dx->status.constant && dt->status.constant && a->status.constant) {
                CHECK_DIV_BY_ZERO(dt->value, ACC_T_ZERO)

                real_t half = os_FloatToReal(0.5f);
                real_t halfA = os_RealMul(&half, &a->value);
                real_t dtdt = os_RealMul(&dt->value, &dt->value);
                real_t halfADtSquared = os_RealMul(&halfA, &dtdt);
                real_t diff = os_RealSub(&dx->value, &halfADtSquared);
                real_t div = os_RealDiv(&diff, &dt->value);
                setVar(eqs, v0, &div);
                change = true;
            } else if (dx->status.constant && dt->status.constant && v->status.constant) {
                CHECK_DIV_BY_ZERO(dt->value, ACC_T_ZERO)

                real_t two = os_FloatToReal(2.0f);
                real_t twoDx = os_RealMul(&two, &dx->value);
                real_t div = os_RealDiv(&twoDx, &dt->value);
                real_t diff = os_RealSub(&div, &v->value);
                setVar(eqs, v0, &diff);
                change = true;
            }
        }
        if (!v->status.constant) {
            if (dt->status.constant && v0->status.constant && a->status.constant) {
                real_t aDt = os_RealMul(&a->value, &dt->value);
                real_t sum = os_RealAdd(&v0->value, &aDt);
                setVar(eqs, v, &sum);
                change = true;
            } else if (dx->status.constant && v0->status.constant && a->status.constant) {
                real_t v0v0 = os_RealMul(&v0->value, &v0->value);
                real_t two = os_FloatToReal(2.0f);
                real_t twoA = os_RealMul(&two, &a->value);
                real_t twoADx = os_RealMul(&twoA, &dx->value);
                real_t sum = os_RealAdd(&v0v0, &twoADx);
                CHECK_NEG_ROOT(sum, ACC_IMPOSSIBLE_INITIAL_V)
                real_t root = os_RealSqrt(&sum);
                setVar(eqs, v, &root);
                change = true;
            } else if (dx->status.constant && v->status.constant && a->status.constant) {
                CHECK_DIV_BY_ZERO(dt->value, ACC_T_ZERO)

                real_t div = os_RealDiv(&dx->value, &dt->value);
                real_t half = os_FloatToReal(0.5f);
                real_t halfA = os_RealMul(&half, &a->value);
                real_t halfADt = os_RealMul(&halfA, &dt->value);
                real_t sum = os_RealAdd(&div, &halfADt);
                setVar(eqs, v, &sum);
                change = true;
            } else if (dx->status.constant && v0->status.constant && v->status.constant) {
                real_t two = os_FloatToReal(2.0f);
                real_t twoDx = os_RealMul(&two, &dx->value);
                real_t div = os_RealMul(&twoDx, &dt->value);
                real_t diff = os_RealSub(&div, &v0->value);
                setVar(eqs, v, &diff);
                change = true;
            }
        }
        if (!a->status.constant) {
            if (dt->status.constant && v0->status.constant && v->status.constant) {
                CHECK_DIV_BY_ZERO(dt->value, ACC_T_ZERO)

                real_t diff = os_RealSub(&v->value, &v0->value);
                real_t div = os_RealDiv(&diff, &dt->value);
                setVar(eqs, a, &div);
                change = true;
            } else if (dx->status.constant && v0->status.constant && v->status.constant) {
                CHECK_DIV_BY_ZERO(dx->value, ACC_X_ZERO)

                real_t vv = os_RealMul(&v->value, &v->value);
                real_t v0v0 = os_RealMul(&v0->value, &v0->value);
                real_t diff = os_RealSub(&vv, &v0v0);
                real_t two = os_FloatToReal(2.0f);
                real_t twoDx = os_RealMul(&two, &dx->value);
                real_t div = os_RealDiv(&diff, &twoDx);
                setVar(eqs, a, &div);
                change = true;
            } else if (dx->status.constant && dt->status.constant && v->status.constant) {
                CHECK_DIV_BY_ZERO(dt->value, ACC_T_ZERO)

                real_t vDt = os_RealMul(&v->value, &dt->value);
                real_t diff = os_RealSub(&dx->value, &vDt);
                real_t dtdt = os_RealMul(&dt->value, &dt->value);
                real_t div = os_RealDiv(&diff, &dtdt);
                real_t negTwo = os_FloatToReal(-2.0f);
                real_t product = os_RealMul(&negTwo, &div);
                setVar(eqs, a, &product);
                change = true;
            } else if (dx->status.constant && dt->status.constant && v0->status.constant) {
                CHECK_DIV_BY_ZERO(dt->value, ACC_T_ZERO)

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

static bool solveSystem(AllEqs *eqs) {
    real_t zero = os_FloatToReal(0.0f);

    for (int i = 0; i < eqs->accCount; i++) {
        for (int j = 0; j < eqs->velCount; j++) {
            VariableValue *acc_dx = eqForField(eqs, ACC, i, DX);
            VariableValue *acc_dt = eqForField(eqs, ACC, i, DT);
            VariableValue *acc_v0 = eqForField(eqs, ACC, i, V0);
            VariableValue *acc_a = eqForField(eqs, ACC, i, A);

            VariableValue *vel_dx = eqForField(eqs, VEL, j, DX);
            VariableValue *vel_dt = eqForField(eqs, VEL, j, DT);
            VariableValue *vel_v = eqForField(eqs, VEL, j, VEL_V);

            int acc_v0_num = eqNumForField(ACC, i, V0);
            int vel_v_num = eqNumForField(VEL, j, VEL_V);
            int acc_dt_num = eqNumForField(ACC, i, DT);
            int vel_dt_num = eqNumForField(VEL, j, DT);

            if (!(acc_dx->status.constant && acc_a->status.constant && vel_dx->status.constant)) continue;
            if (!(!acc_v0->status.constant && acc_v0->status.variable &&
                  os_RealCompare(&acc_v0->eq.intercept, &zero) == 0))
                continue;
            if (!(!vel_v->status.constant && vel_v->status.variable &&
                  os_RealCompare(&vel_v->eq.intercept, &zero) == 0))
                continue;
            if (!(acc_v0->eq.varNum == vel_v->eq.varNum || acc_v0->eq.varNum == vel_v_num ||
                  vel_v->eq.varNum == acc_v0_num))
                continue;
            bool acc_dt_isVar = acc_dt->status.variable;
            bool vel_dt_isVar = vel_dt->status.variable;
            if (!(!acc_dt->status.constant && (!acc_dt_isVar || os_RealCompare(&acc_dt->eq.intercept, &zero) == 0)))
                continue;
            if (!(!vel_dt->status.constant && (!vel_dt_isVar || os_RealCompare(&vel_dt->eq.intercept, &zero) == 0)))
                continue;
            if (!(
                    (acc_dt_isVar && vel_dt_isVar && vel_dt->eq.varNum == acc_dt->eq.varNum) ||
                    (vel_dt_isVar && vel_dt->eq.varNum == acc_dt_num) ||
                    (acc_dt_isVar && acc_dt->eq.varNum == vel_dt_num)))
                continue;

            if (os_RealCompare(&acc_a->value, &zero) == 0) continue;
            real_t half = os_FloatToReal(0.5f);
            real_t halfAccA = os_RealMul(&half, &acc_a->value);

            real_t v0VelDx = os_RealMul(&acc_v0->eq.coeff, &vel_dx->value);
            real_t velVVelDt = os_RealMul(&vel_v->eq.coeff, &vel_dt->eq.coeff);
            if (os_RealCompare(&velVVelDt, &zero) == 0) continue;
            real_t div = os_RealDiv(&v0VelDx, &velVVelDt);
            real_t diff = os_RealSub(&acc_dx->value, &div);
            real_t div2 = os_RealDiv(&diff, &halfAccA);
            if (os_RealCompare(&div2, &zero) < 0) continue;
            real_t root = os_RealSqrt(&div2);
            setVar(eqs, acc_dt, &root);

            return true;
        }
    }

    return false;
}

void solve(AllEqs *eqs) {
    illegalConfig = (IllegalConfig) {
            .type = NONE
    };

    do {
        bool change = true;
        while (change) {
            change = false;

            change = updateVelSums(eqs);
            if (illegalConfig.type != NONE) break;
            change = change || updateConstantVel(eqs);
            if (illegalConfig.type != NONE) break;
            change = change || updateConstantAcc(eqs);
            if (illegalConfig.type != NONE) break;

            change = change || updateVars(eqs);
        }
    } while(solveSystem(eqs));

    if (illegalConfig.type != NONE) {
        clearCalculatedValues(eqs);
        gfx_FillScreen(0xFF);
        txt_writeStrAtPos(0, 0, "Impossible values in ");
        EqType eqType = illegalConfigEqType[illegalConfig.type];
        if (eqType == VEL_SUM) {
            txt_writeStr(eqs->velSumNames[illegalConfig.eqNum]);
        } else if (eqType == VEL) {
            txt_writeStr(eqs->velNames[illegalConfig.eqNum]);
        } else if (eqType == ACC) {
            txt_writeStr(eqs->accNames[illegalConfig.eqNum]);
        }
        txt_writeStr(".");
        txt_writeStrAtPos(0, 2, illegalConfigDescriptions[illegalConfig.type]);
        txt_writeStrAtPos(0, 4, "Press any key.");

        while (!os_GetCSC());
    }
}
