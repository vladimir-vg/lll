#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* for env_buf */
#include "lll_interpreter.h"
#include "lll_utils.h"

const char *error_messages[] = {
    /* 000 */
    "Length of symbol:\n ``%s''\nmore then 60.\n",

    /* 001 */
    "Length of symbol ``%s'' equal 0. How you do that?\n",

    /* 002 */
    "Incorrect symbol name: ``%s''\n",

    /* 003 */
    "Function ``%s'' get not enough arguments.\n",

    /* 004 */
    "Function ``%s'' get too many arguments.\n",

    /* 005 */
    "Inner ``%s'' function get a non-nil terminated list.\n",

    /* 006 */
    "Inner ``%s'' function get object with unknown type_code.\n",

    /* 007 */
    "Inner ``%s'' function get a non-symbol arg as function name.\n",

    /* 008 */
    "Symbol ``%s'' binded to non-bf, but tries to be called as function.\n",

    /* 009 */
    "Function ``%s'' got non-list argument.\n",

    /* 010 */
    "``%s'' reference to NULL.\n",

    /* 011 */
    "%s not implemented yet.\n",

    /* 012 */
    "Only builtin-functions and lambda-functions can evaluated.\n",

    /* 013 */
    "Function ``%s'' get an EOF.\n",

    /* 014 */
    "Error at reading: %s.\n",

    /* 015 */
    "Unknown ``%s'' function\n",

    /* 016 */
    "Arithmetic function ``%s'' got non-number\n",

    /* 017 */
    "Arguments count does not match in ``%s''\n",

    /* 018 */
    "Lambda constructor get equal symbol as argument-names\n",

    /* 019 */
    "Lambda get non-symbol as argument-name\n",

    /* 020 */
    "Non-symbol object tries to binding\n",

    /* 021 */
    "Incorrect '%s' form\n",

    /* 022 */
    "Symbol ``%s'' is undefined\n",

    /* 023 */
    "Error at eval: ``%s''\n",

    /* 024 */
    "Error: %s\n"
};

void
lll_fatal_error(uint32_t error_code, const char *message, const char *filename, const uint32_t line) {
    fprintf(stderr, "[lll/%s:%d]Fatal: ", filename, line);
    if (message == NULL) {
        fprintf(stderr, error_messages[error_code]);
    }
    else {
        fprintf(stderr, error_messages[error_code], message);
    }
    exit(0);
}

void
lll_error(uint32_t error_code, const char *message, const char *filename, const uint32_t line) {
    fprintf(stderr, "[lll/%s:%d]: ", filename, line);
    if (message == NULL) {
        fprintf(stderr, error_messages[error_code]);
    }
    else {
        fprintf(stderr, error_messages[error_code], message);
    }
    longjmp(env_buf, 1);
}

/* returns copy of lowercase version string.
 */
char *
lll_to_lowercase(const char *string) {
    int length = strlen(string);
    char *result = (char *) malloc(length + 1);
    for (int i = 0; i < length; ++i) {
        if (islower(string[i])) {
            result[i] = tolower(string[i]);
        }
        else {
            result[i] = tolower(string[i]);
        }
    }
    result[length] = '\0';
    return result;
}
