//
// Created by danie on 12/20/2021.
//

#ifndef KINCALC_SOLVE_H
#define KINCALC_SOLVE_H

typedef enum {
    NONE, VEL_T_ZERO, VEL_ZERO, ACC_ZERO, ACC_T_ZERO, ACC_X_ZERO, ACC_VEL_SUM, VY_GREATER_THAN_V, VX_GREATER_THAN_V,
    ACC_IMPOSSIBLE_FINAL_V, ACC_IMPOSSIBLE_INITIAL_V
} ILLEGAL_CONFIG_TYPE;

static EqType illegalConfigEqType[] = {
        0, VEL, VEL, ACC, ACC, ACC, ACC, VEL_SUM, VEL_SUM, ACC, ACC
};

static char *illegalConfigDescriptions[] = {
        "",
        "\x16t may not be zero.",
        "v may not be zero.",
        "a may not be zero.",
        "\x16t may not be zero.",
        "\x16x may not be zero.",
        "v0 + v may not be zero.",
        "vy must be less than v.",
        "vx must be less than v.",
        "Impossible v.",
        "Impossible v0."
};


typedef struct {
    ILLEGAL_CONFIG_TYPE type;
    int eqNum;
} IllegalConfig;

void solve(AllEqs *eqs);

#endif //KINCALC_SOLVE_H
