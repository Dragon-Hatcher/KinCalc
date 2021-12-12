//
// Created by danie on 12/12/2021.
//

#include "textInput.h"

#include <tice.h>
#include "draw_simple_text.h"

void textInput(char *buffer, int maxLength, int minLength, int row, int col) {
    int cursor = 0;
    buffer[cursor] = '\0';

    const char *alphaChars = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";

    do {
        sk_key_t key = os_GetCSC();
        if (key == sk_Enter && cursor >= minLength) return;

        if (key == sk_Del && cursor != 0) {
            cursor--;
            drawAtCharPos(col + cursor, row, " ");
            buffer[cursor] = '\0';
        }
        if (alphaChars[key] && cursor != maxLength) {
            buffer[cursor++] = alphaChars[key];
            buffer[cursor] = '\0';
            drawAtCharPos(col, row, buffer);
        }

    } while (true);
}