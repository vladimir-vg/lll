#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "lll_read.h"
#include "lll_symtable.h"
#include "lll_base_funcs.h"
#include "lll_types.h"
#include "lll_utils.h"

#define BUFFER_SIZE 512

char buffer[BUFFER_SIZE];
uint32_t length;
/* used in recursive s-exp parsing procedure */
char *strrec;

/*
  first -- read to string one expression.
  read while not found some whitespace,
  or while parentheses are unbalanced.
 */
struct lll_object *
lll_read(FILE * fd) {

        extern struct lll_object *parse_atom(char *);
        extern struct lll_object *parse_sexp();

        /* for parentheses counting. */
        uint32_t deep = 0;
        char c;
        length = 0;
        bool is_sexp = false;

        c = getc(fd);
        while (isspace(c)) {
                c = getc(fd);
        }
        int last_space = 0;

        /* then we just read all chars before whitespace or EOF */
        if (c != '(') {
                do {
                        buffer[length++] = c;
                        if (length == BUFFER_SIZE - 1) {
                                lll_error(14, "too big input", __FILE__, __LINE__);
                        }
                        c = getc(fd);
                } while ((feof(fd) != 0) || !(last_space = isspace(c)));
        }
        else {
                is_sexp = true;
                deep = 1;
                do {
                        buffer[length++] = c;
                        if (length == BUFFER_SIZE - 1) {
                                lll_error(14, "too big input", __FILE__, __LINE__);
                        }

                        c = getc(fd);
                        if (c == '(') {
                                deep++;
                        }
                        if (c == ')') {
                                deep--;
                        }
                } while (deep != 0);
                buffer[length++] = c;
        }
        if (!last_space) {
                length++;
        }
        buffer[length] = '\0';

        if (is_sexp) {
                strrec = buffer;
                return parse_sexp();
        }
        else {
                return parse_atom(buffer);
        }
}

struct lll_object *
parse_atom(char *str) {
        extern bool correct_integer_p(char *);
        extern bool correct_char_p(char *);
        extern bool correct_string_p(char *);
        extern bool correct_symbol_p(char *);

        int length = strlen(str);

        if (correct_integer_p(str)) {
                int32_t i;
                sscanf(str, "%d", &i);
                return lll_cinteger32(i);
        }
        else if (correct_char_p(str)) {
                return lll_cchar(str[2]);
        }
        else if (correct_string_p(str)) {
                for (int i = length - 1; i > 0; --i) {
                        if (str[i] == '"') {
                                str[i] = '\0';
                                break;
                        }
                }
                char *nstr = (char *) malloc(length);
                strcpy(nstr, &str[1]);
                return lll_cstring(nstr);
        }
        else if (correct_symbol_p(str)) {
                return lll_get_symbol(str);
        }
        else {
                // to del
                printf("parse:'%s'\n", str);
                lll_error(14, "Couldn't parse string", __FILE__, __LINE__);
        }

        return NULL;
}

struct lll_object *
parse_sexp() {
        printf("strrec:'%s'\n", strrec);
        extern void pass_whitespace(char **);
        char *nstr;

        pass_whitespace(&strrec);

        if (strrec[0] != '(') {
                lll_error(14, "First char was not '(' at parsing s-exp", __FILE__, __LINE__);
        }

        strrec++;
        pass_whitespace(&strrec);

        /* we have ()  */
        if (strrec[0] == ')') {
                strrec++;
                return lll_get_symbol("nil");
        }

        struct lll_object *tmp = NULL;
        struct lll_object *result = MALLOC_STRUCT(lll_object);
        result->d.pair = MALLOC_STRUCT(lll_pair);
        result->type_code = LLL_PAIR;
        result->d.pair->car = NULL;
        result->d.pair->cdr = NULL;

        while (strrec[0] != '\0') {
                int n = 0;
                while (strrec[0] != '\0') {
                        if (isspace(strrec[n])) {
                                strrec[n] = '\0';
                                nstr = &strrec[n + 1];
                                tmp = parse_atom(strrec);
                                lll_append_to_list(result, tmp);
                                strrec = nstr;
                                pass_whitespace(&strrec);
                                n = 0;
                        }
                        else if (strrec[n] == '(') {
                                strrec[n] = '\0';
                                nstr = &strrec[n];
                                if (strrec[0] != '\0') {
                                        tmp = parse_atom(strrec);
                                        lll_append_to_list(result, tmp);
                                }
                                else {
                                        break;
                                }
                                strrec = nstr;
                                strrec[0] = '(';
                                tmp = parse_sexp();
                                lll_append_to_list(result, tmp);
                                pass_whitespace(&strrec);
                                break;
                        }
                        else if (strrec[n] == ')') {
                                if (strrec[0] != '\0') {
                                        strrec[n] = '\0';
                                        nstr = &strrec[n + 1];
                                        tmp = parse_atom(strrec);
                                        lll_append_to_list(result, tmp);
                                }
                                else {
                                        break;
                                }
                                strrec = nstr;
                                pass_whitespace(&strrec);
                                return result;
                        }
                        else {
                                n++;
                        }
                }
        }

        return result;
}

void
pass_whitespace(char **str) {
        while (isspace((*str)[0])) {
                (*str)++;
        }
}

bool
correct_integer_p(char *str) {
        uint32_t n = 0;
        if ((str[0] == '-') || isdigit(str[0])) {
                n++;
                while (str[n] != '\0') {
                        if (!isdigit(str[n])) {
                                return false;
                        }
                        n++;
                }
                return true;
        }
        else {
                return false;
        }
}

bool
correct_char_p(char *str) {
        if (str[0] == '#' && str[1] == '\\' && str[3] == '\0') {
                return true;
        }
        return false;
}

bool
correct_string_p(char *str) {
        int length = strlen(str);
        if (str[0] == '"' && str[length - 1] == '"') {
                return true;
        }
        return false;
}

bool
correct_symbol_p(char *str) {
        uint32_t n = 0;
        if (isspace(str[0]) || isdigit(str[0]) || !isallowd(str[0])) {
                return false;
        }
        n++;
        while (str[n] != '\0') {
                if (isspace(str[n]) || !isallowd(str[0])) {
                        return false;
                }
                n++;
        }
        return true;
}
