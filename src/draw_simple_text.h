//
// Created by danie on 12/11/2021.
//

#ifndef KINCALC_DRAW_SIMPLE_TEXT_H
#define KINCALC_DRAW_SIMPLE_TEXT_H

#define START_HEIGHT LINE_GAP
#define START_WIDTH CHAR_GAP

int txt_charsLeftInRow();

void txt_resetX();
void txt_resetY();
void txt_reset();

void txt_setCol(int chars);
void txt_moveCols(int cols);
void txt_setRow(int rows);
void txt_newline();

void txt_writeStr(const char *str);
void txt_WriteChar(char toWrite);
void txt_WriteStrAtCol(int charX, const char *str);
void txt_WriteCharAtCol(int charX, char charToWrite);
void txt_writeStrAtPos(int charX, int charY, const char *str);

char lowercase(char toLowercase);

#endif //KINCALC_DRAW_SIMPLE_TEXT_H
