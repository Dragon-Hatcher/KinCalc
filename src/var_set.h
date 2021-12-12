//
// Created by danie on 12/11/2021.
//

#ifndef KINCALC_VAR_SET_H
#define KINCALC_VAR_SET_H

#include <tice.h>

typedef struct {
    uint8_t id;
    bool resolved;
    real_t value;
} Variable;

typedef struct {
    real_t coeff;
    real_t intercept;
    Variable *variable;
} LinearEq;

typedef enum QuadStrategy {
    MAX, MIN
} QuadStrategy;

typedef struct {
    LinearEq dx;
    LinearEq dt;
    LinearEq v0;
    LinearEq v;
    LinearEq a;
    QuadStrategy quadStrategy;
} VarSet;

bool isVarSetResolved(VarSet *varSet);

bool isLinearEqResolved(LinearEq *eq);
real_t getValueOfLinearEq(LinearEq *eq);
void setValueOfLinearEq(LinearEq *eq, real_t value);

#endif //KINCALC_VAR_SET_H
