//
// Created by danie on 12/12/2021.
//

#ifndef KINCALC_MM_EQS_H
#define KINCALC_MM_EQS_H

#include <tice.h>

typedef struct {
    bool determined;
    real_t coeff;
    int varNum;
    real_t intercept;
} LinearEq;

#define NAME_CHAR_COUNT 2
#define NAME_SIZE (NAME_CHAR_COUNT + 1)

typedef char Name[NAME_SIZE];

typedef enum EqType {
    ACC = 0, VEL, VEL_SUM, FREE_VAR
} EqType;

typedef enum {
    DX = 0, DT = 1, V0 = 2, ACC_V = 3, A = 4,
    VEL_V = 2,
    SUM_V = 0, VX = 1, VY = 2,
    VAR = 0
} Field;

#define ACC_VAR_COUNT 10
#define VEL_VAR_COUNT 10
#define VEL_SUM_VAR_COUNT 5
#define FREE_VAR_VAR_COUNT 5

#define ACC_FIELD_COUNT 5
#define VEL_FIELD_COUNT 3
#define VEL_SUM_FIELD_COUNT 3
#define FREE_VAR_FIELD_COUNT 1

#define ACC_TOT_VAR_COUNT (ACC_VAR_COUNT * ACC_FIELD_COUNT)
#define VEL_TOT_VAR_COUNT (VEL_VAR_COUNT * VEL_FIELD_COUNT)
#define VEL_SUM_TOT_VAR_COUNT (VEL_SUM_VAR_COUNT * VEL_SUM_FIELD_COUNT)
#define FREE_VAR_TOT_VAR_COUNT (FREE_VAR_VAR_COUNT * FREE_VAR_FIELD_COUNT)

static int fieldCounts[] = {ACC_FIELD_COUNT, VEL_FIELD_COUNT, VEL_SUM_FIELD_COUNT, FREE_VAR_FIELD_COUNT};

static int varStartOffset[] = {
        0,
        ACC_TOT_VAR_COUNT,
        ACC_TOT_VAR_COUNT + VEL_TOT_VAR_COUNT,
        ACC_TOT_VAR_COUNT + VEL_TOT_VAR_COUNT + VEL_SUM_TOT_VAR_COUNT};

#define VARIABLE_COUNT (ACC_TOT_VAR_COUNT + VEL_TOT_VAR_COUNT + VEL_SUM_TOT_VAR_COUNT + FREE_VAR_TOT_VAR_COUNT)

typedef struct {
    LinearEq variables[VARIABLE_COUNT];
    uint8_t accCount;
    Name accNames[ACC_VAR_COUNT];
    uint8_t velCount;
    Name velNames[VEL_VAR_COUNT];
    uint8_t velSumCount;
    Name velSumNames[VEL_SUM_VAR_COUNT];
    uint8_t freeVarCount;
    Name freeVarNames[FREE_VAR_VAR_COUNT];
} AllEqs;

LinearEq *eqForField(AllEqs *eqs, EqType eqType, int eqNum, Field field);

#endif //KINCALC_MM_EQS_H