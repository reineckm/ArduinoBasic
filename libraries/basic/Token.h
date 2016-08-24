#ifndef TOKEN_H
#define TOKEN_H
#include "stdint.h"

enum __attribute__ ((__packed__)) TokenType {NUMBER, STRING, OPERATOR, KEYWORD, VARIABLE, RELOP, LINENUMBER, EMPTY, PROGEND};
enum __attribute__ ((__packed__)) Keywords {PRINT, LET, GOTO, IF, ASK, END, RND, FOR, NEXT, TO, PLINE};

struct Token {
    int16_t __attribute__ ((__packed__)) value;
    char __attribute__ ((__packed__)) type;
};
#endif // TOKEN_H
