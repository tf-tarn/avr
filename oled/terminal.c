#include <string.h>
#include "font.h"
#include "oled.h"
#include "uart.h"

#define FONTWIDTH 6
#define FONTHEIGHT 8

#define NCOLS (SCREENWIDTH / FONTWIDTH)
#define NROWS (SCREENHEIGHT / FONTHEIGHT)

static unsigned char term_row = 0;
static unsigned char term_col = 0;

void term_row_(unsigned char row) {
    if (row < NROWS) {
        term_row = row;
    }
}

void term_set_row(unsigned char row) {
    if (row < NROWS) {
        term_row = row;
    }
}

void term_set_col(unsigned char col) {
    if (col < NCOLS) {
        col = term_col;
    }
}

void term_write_impl(unsigned char c) {
    unsigned int cc;
    if (c < ' ') {
        cc = 70;
    } else if (c < 'a') {
        cc = c - ' ';
    } else if (c <= 'z') {
        cc = c - ' ' - 0x20; // shift lower-case to upper
    } else if (c < 0x80) {
        cc = c - '{' + 65;
    } else {
        cc = 70;
    }
    cc *= 5;
    for (int f = 0; f < 5; ++f) {
        oled_data(font[cc+f]);
    }
    oled_data(0); // 1px space between characters
}

static void newline() {
    oled_command(0x22); // set page address
    oled_command(term_row);
    oled_command(term_row);
}
static void return_carriage() {
    oled_command(0x21); // set column address
    oled_command(0);
    oled_command(SCREENWIDTH - 1);
}

void term_write(unsigned char c) {
    if (c == '\r') {
        term_col = 0;
        return_carriage();
        return;
    }

    if (c == '\n') {
        ++term_row;
        newline();
        return;
    }

    if (term_col < NCOLS) {
        ++term_col;
    } else {
        // Automatically \r and then \n
        term_row = (term_row + 1) % NROWS;
        term_col = 1;
        newline();
        return_carriage();
    }
    term_write_impl(c);
}

void term_write_sn(const char *s, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        term_write(s[i]);
    }
}

void term_write_s(const char *s) {
    term_write_sn(s, strlen(s));
}

static const char *digits = "0123456789abcdef";

void term_write_n(int n) {
    if (n < 0) {
        term_write('-');
        n = -n;
    }

    if (n >= 10000) {
        term_write(digits[(n / 10000) % 10]);
    }
    if (n >= 1000) {
        term_write(digits[(n / 1000) % 10]);
    }
    if (n >= 100) {
        term_write(digits[(n / 100) % 10]);
    }
    if (n >= 10) {
        term_write(digits[(n / 10) % 10]);
    }
    term_write(digits[n % 10]);
}


void term_clear() {
    oled_command(0x22); // set page address
    oled_command(0);
    oled_command(7);
    oled_command(0x21); // set column address
    oled_command(0);
    oled_command(SCREENWIDTH-1);

    oled_clear();

    term_col = 0;
    term_row = 0;
    return_carriage();
    newline();
}
