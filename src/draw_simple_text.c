//
// Created by danie on 12/11/2021.
//

#include "draw_simple_text.h"

#include <fontlibc.h>
#include <string.h>
#include "init_font.h"

static int pixelsOver = START_WIDTH;
static int pixelsDown = START_HEIGHT;

void txt_resetX() { pixelsOver = START_WIDTH; }

void txt_resetY() { pixelsDown = START_HEIGHT; }

void txt_reset() {
    txt_resetX();
    txt_resetY();
}

void txt_setCol(int chars) { pixelsOver = START_WIDTH + chars * CHAR_WIDTH; }

void txt_moveCols(int cols) { pixelsOver += cols * CHAR_WIDTH; }

void txt_setRow(int rows) { pixelsDown = START_HEIGHT + rows * LINE_HEIGHT; }

void txt_newline() { pixelsDown += LINE_HEIGHT; }

void txt_writeStr(const char *str) {
    fontlib_SetCursorPosition(pixelsOver, pixelsDown);
    fontlib_DrawString(str);
    pixelsOver += (int) strlen(str) * CHAR_WIDTH;
}

void txt_WriteChar(char toWrite) {
    char str[2] = {toWrite, '\0'};
    txt_writeStr(str);
}

void txt_WriteStrAtCol(int charX, const char *str) {
    txt_setCol(charX);
    txt_writeStr(str);
}

void txt_WriteCharAtCol(int charX, char charToWrite) {
    txt_setCol(charX);
    char str[2] = {charToWrite, '\0'};
    txt_writeStr(str);
}


void txt_writeStrAtPos(int charX, int charY, const char *str) {
    txt_setCol(charX);
    txt_setRow(charY);
    txt_writeStr(str);
}

char lowercase(char toLowercase) {
    if (toLowercase >= 0x41 && toLowercase <= 0x5A) {
        return (char)(toLowercase + 0x20);
    } else {
        return toLowercase;
    }
}