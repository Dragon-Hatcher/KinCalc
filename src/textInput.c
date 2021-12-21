//
// Created by danie on 12/12/2021.
//

#include "textInput.h"

#include <tice.h>
#include "draw_simple_text.h"

void textInput(char *buffer, int maxLength, int minLength, int row, int col) {
    int cursor = 0;
    buffer[cursor] = '\0';

    const char *alphaChars = "\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0?\xB6VQLG\0\0\0ZUPKFC\0\0YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";

    do {
        sk_key_t key = os_GetCSC();
        if (key == sk_Enter && cursor >= minLength) return;

        if (key == sk_Del && cursor != 0) {
            cursor--;
            txt_writeStrAtPos(col + cursor, row, " ");
            buffer[cursor] = '\0';
        }
        if (alphaChars[key] && cursor != maxLength) {
            buffer[cursor++] = alphaChars[key];
            buffer[cursor] = '\0';
            txt_writeStrAtPos(col, row, buffer);
        }

    } while (true);
}