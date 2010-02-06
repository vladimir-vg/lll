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
        "Inner ``%s'' function get a non-null terminated list.\n",

        /* 006 */
        "Inner ``%s'' function get object with unknown type_code.\n",

        /* 007 */
        "Inner ``%s'' function get a non-symbol arg as function name.\n",

        /* 008 */
        "Symbol ``%s'' binded to non-bf, but tries to be called as function.\n",

        /* 009 */
        "Function ``%s'' got non-list argument.\n",

        /* 010 */
        "``%s'' reference to NULL.\n"
};

void
lll_fatal_error(uint32_t error_code, const char *message) {
        fprintf(stderr, "[lll]Fatal: ");
        if (message == NULL) {
                fprintf(stderr, error_messages[error_code]);
        }
        else {
                fprintf(stderr, error_messages[error_code], message);
        }
        exit(0);
}

void
lll_error(uint32_t error_code, const char *message) {
        fprintf(stderr, "[lll]: ");
        if (message == NULL) {
                fprintf(stderr, error_messages[error_code]);
        }
        else {
                fprintf(stderr, error_messages[error_code], message);
        }
        longjmp(env_buf, 1);
}

void
lll_warning(const char *message) {
        fprintf(stderr, "[lll]Warning: %s\n", message);
}

/* if conversion not need -- just return arg.
   else malloc mem for lowercase-version.
 */
char *
lll_to_lowercase(const char *string, bool * allocated) {
        int length = strlen(string);
        (*allocated) = false;
        char *result = (char *) string;
        for (int i = 0; i < length; ++i) {
                if (isalpha((string)[i]) && isupper(string[i])) {
                        if (*allocated) {
                                result[i] = tolower(string[i]);
                        }
                        else {
                                result = (char *) malloc(strlen(string) + 1);
                                strcpy(result, string);
                                result[i] = tolower(string[i]);
                                (*allocated) = true;
                        }
                }
        }

        /* it is safe, because we control with 'allocated' variable
           in get_binded_object bind_object functions.
         */
        return result;
}
