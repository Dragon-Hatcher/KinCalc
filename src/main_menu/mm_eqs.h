//
// Created by danie on 12/12/2021.
//

#ifndef KINCALC_MM_EQS_H
#define KINCALC_MM_EQS_H

#include <tice.h>

typedef enum {
    CONSTANT_ACCEL = 0, CONSTANT_VEL, VELOCITY_SUM, FREE_VAR
} EqType;

typedef struct {
    EqType type;
    int eqNum;
    int eqField;
    bool determined;
    real_t value;
} Variable;

typedef enum {
    CONSTANT, VAR, COEFF, OFFSET, LINEAR, NONE
} ParamType;

typedef struct {
    ParamType type;
    real_t coeff;
    Variable *var;
    real_t offset;
} Param;

#define NAME_CHAR_COUNT 2
#define NAME_SIZE (NAME_CHAR_COUNT + 1)

// Const Acceleration --------------------------------------------------------------------------------------------------

typedef struct {
    char name[NAME_SIZE];
    Param dx;
    Param dt;
    Param v0;
    Param v;
    Param a;
} ConstAcc;

#define CONST_ACC_COUNT 10

typedef struct {
    int count;
    ConstAcc accs[CONST_ACC_COUNT];
} ConstAccs;

// Const Vel -----------------------------------------------------------------------------------------------------------

typedef struct {
    char name[NAME_SIZE];
    Param dx;
    Param dt;
    Param v;
} ConstVel;

#define CONST_VEL_COUNT 10

typedef struct {
    int count;
    ConstVel vels[CONST_VEL_COUNT];
} ConstVels;

// Vel Sums ------------------------------------------------------------------------------------------------------------

typedef struct {
    char name[NAME_SIZE];
    Param x;
    Param y;
} VelSum;

#define VEL_SUM_COUNT 5

typedef struct {
    int count;
    VelSum sums[VEL_SUM_COUNT];
} VelSums;

// Free Vars -----------------------------------------------------------------------------------------------------------

typedef struct {
    char name[NAME_SIZE];
    Variable *var;
} FreeVar;

#define FREE_VAR_COUNT 5

typedef struct {
    int count;
    FreeVar vars[FREE_VAR_COUNT];
} FreeVars;

#define VARIABLE_COUNT (CONST_ACC_COUNT * 5 + CONST_VEL_COUNT * 3 + VEL_SUM_COUNT * 2 + FREE_VAR_COUNT)

typedef struct {
    Variable variables[VARIABLE_COUNT];
} Vars;

typedef struct {
    ConstAccs *accs;
    ConstVels *vels;
    VelSums *velSums;
    FreeVars *freeVars;
    Vars *vars;
} AllEqs;

Param emptyParamValue(void);

Variable *varForParam(AllEqs *eqs, Param *param);
Variable *varForTypeAndNum(AllEqs *eqs, EqType type, int eqNum, int varNum);

#endif //KINCALC_MM_EQS_H
