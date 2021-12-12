//
// Created by danie on 12/12/2021.
//

#ifndef KINCALC_NEW_EQ_H
#define KINCALC_NEW_EQ_H

#include "tice.h"

typedef enum {
    CONSTANT_ACCEL = 0, CONSTANT_VEL, VELOCITY_SUM, FREE_VAR
} EqType;

#define NAME_LENGTH 2

typedef struct {
    char name[NAME_LENGTH + 1];
} FreeVar;

#define FREE_VAR_COUNT 10

typedef struct {
    int count;
    FreeVar vars[FREE_VAR_COUNT];
} FreeVars;

typedef struct {
    FreeVars *freeVars;
} AllEqs;

void newEq(AllEqs *eqs);

#endif //KINCALC_NEW_EQ_H
