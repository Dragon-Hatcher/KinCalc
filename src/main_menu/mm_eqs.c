//
// Created by danie on 12/12/2021.
//

#include "mm_eqs.h"

LinearEq *eqForField(AllEqs *eqs, EqType eqType, int eqNum, Field field) {
    return &eqs->variables[varStartOffset[eqType] + eqNum * fieldCounts[eqType] + field];
}

//Param emptyParamValue(void) {
//    return (Param) {.type = NONE};
//}
//
//Variable *varForTypeAndNum(AllEqs *eqs, EqType type, int eqNum, int varNum) {
//    Variable *vars = eqs->vars->variables;
//    if (type == CONSTANT_ACCEL) return &vars[eqNum * 5 + varNum];
//    if (type == CONSTANT_VEL) return &vars[eqs->accs->count * 5 + eqNum * 3 + varNum];
//    if (type == VELOCITY_SUM) return &vars[eqs->accs->count * 5 + eqs->vels->count * 3 + eqNum * 3 + varNum];
//    return &vars[eqs->accs->count * 5 + eqs->vels->count * 3 + eqs->velSums->count * 2 + eqNum];
//}
//
//Variable *varForParam(AllEqs *eqs, Param *param) {
//    for (int i = 0; i < eqs->accs->count; i++) {
//        if (&eqs->accs->accs[i].dx == param) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 0);
//        if (&eqs->accs->accs[i].dt == param) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 1);
//        if (&eqs->accs->accs[i].v0 == param) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 2);
//        if (&eqs->accs->accs[i].v == param) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 3);
//        if (&eqs->accs->accs[i].a == param) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 4);
//    }
//    for (int i = 0; i < eqs->vels->count; i++) {
//        if (&eqs->vels->vels[i].dx == param) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 0);
//        if (&eqs->vels->vels[i].dt == param) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 1);
//        if (&eqs->vels->vels[i].v == param) return varForTypeAndNum(eqs, CONSTANT_ACCEL, i, 2);
//    }
//    for (int i = 0; i < eqs->velSums->count; i++) {
//        if (&eqs->velSums->sums[i].x == param) return varForTypeAndNum(eqs, VELOCITY_SUM, i, 0);
//        if (&eqs->velSums->sums[i].y == param) return varForTypeAndNum(eqs, VELOCITY_SUM, i, 1);
//    }
//    return NULL;
//}
