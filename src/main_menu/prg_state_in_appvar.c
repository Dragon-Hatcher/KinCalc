//
// Created by danie on 12/12/2021.
//

#include "prg_state_in_appvar.h"

#include <fileioc.h>
#include "mm_eqs.h"
#include "main_menu.h"
#include <debug.h>

//static void initVars(Vars *vars) {
//    int varNum = 0;
//    for (int i = 0; i < ACC_VAR_COUNT; i++) {
//        vars->variables[varNum++] = (Variable) {.type = CONSTANT_ACCEL, .eqNum = i, .eqField = 0, .determined = false};
//        vars->variables[varNum++] = (Variable) {.type = CONSTANT_ACCEL, .eqNum = i, .eqField = 1, .determined = false};
//        vars->variables[varNum++] = (Variable) {.type = CONSTANT_ACCEL, .eqNum = i, .eqField = 2, .determined = false};
//        vars->variables[varNum++] = (Variable) {.type = CONSTANT_ACCEL, .eqNum = i, .eqField = 3, .determined = false};
//        vars->variables[varNum++] = (Variable) {.type = CONSTANT_ACCEL, .eqNum = i, .eqField = 4, .determined = false};
//    }
//    for (int i = 0; i < VEL_VAR_COUNT; i++) {
//        vars->variables[varNum++] = (Variable) {.type = CONSTANT_VEL, .eqNum = i, .eqField = 0, .determined = false};
//        vars->variables[varNum++] = (Variable) {.type = CONSTANT_VEL, .eqNum = i, .eqField = 1, .determined = false};
//        vars->variables[varNum++] = (Variable) {.type = CONSTANT_VEL, .eqNum = i, .eqField = 2, .determined = false};
//    }
//    for (int i = 0; i < VEL_SUM_VAR_COUNT; i++) {
//        vars->variables[varNum++] = (Variable) {.type = VELOCITY_SUM, .eqNum = i, .eqField = 0, .determined = false};
//        vars->variables[varNum++] = (Variable) {.type = VELOCITY_SUM, .eqNum = i, .eqField = 1, .determined = false};
//    }
//    for (int i = 0; i < FREE_VAR_VAR_COUNT; i++) {
//        vars->variables[varNum++] = (Variable) {.type = FREE_VAR, .eqNum = i, .eqField = 0, .determined = false};
//    }
//}
//
//#define ALL_SIZE (sizeof(ConstAccs) + sizeof(ConstVels) + sizeof(VelSums) + \
//sizeof(FreeVars) + sizeof(Vars) + sizeof(AllEqs) + sizeof(MMState))
//
//MMState *initMMState(ti_var_t stateVar) {
//    ti_Resize(ALL_SIZE, stateVar);
//    ti_Seek(0, 0, stateVar);
//
//    MMState state;
//    MMState *statePtr = ti_GetDataPtr(stateVar);
//    dbg_sprintf(dbgout, "init: %p\n", statePtr);
//    ti_Write(&state, sizeof state, 1, stateVar);
//
//    ConstAccs accs = {.count = 0};
//    ConstAccs *accsPtr = ti_GetDataPtr(stateVar);
//    ti_Write(&accs, sizeof accs, 1, stateVar);
//
//    ConstVels vels = {.count = 0};
//    ConstVels *velsPtr = ti_GetDataPtr(stateVar);
//    ti_Write(&vels, sizeof vels, 1, stateVar);
//
//    VelSums velSums = {.count = 0};
//    VelSums *velSumsPtr = ti_GetDataPtr(stateVar);
//    ti_Write(&velSums, sizeof velSums, 1, stateVar);
//
//    FreeVars freeVars = {.count = 0};
//    FreeVars *freeVarsPtr = ti_GetDataPtr(stateVar);
//    ti_Write(&freeVars, sizeof freeVars, 1, stateVar);
//
//    Vars vars;
//    Vars *varsPtr = ti_GetDataPtr(stateVar);
//    ti_Write(&vars, sizeof vars, 1, stateVar);
//    initVars(varsPtr);
//
//    AllEqs eqs = {
//            .accs = accsPtr,
//            .vels = velsPtr,
//            .velSums = velSumsPtr,
//            .freeVars = freeVarsPtr,
//            .vars = varsPtr
//    };
//    AllEqs *eqsPtr = ti_GetDataPtr(stateVar);
//    ti_Write(&eqs, sizeof eqs, 1, stateVar);
//
//    *statePtr = (MMState) {
//            .eqs = eqsPtr,
//            .selectedRow = 0,
//            .scroll = 0,
//            .editingParam = NULL
//    };
//
//    return statePtr;
//}
//