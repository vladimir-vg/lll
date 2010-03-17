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

struct lll_object *parse_atom(char *, char *);
struct lll_object *parse_sexp(char *, char *);
void unparent(char **, int);
int sexp_length(char *);
void pass_whitespace(char **);

bool correct_integer_p(char *);
bool correct_char_p(char *);
bool correct_string_p(char *);
bool correct_symbol_p(char *);

/*
  first -- read to string one expression.
  read while not found some whitespace,
  or while parentheses are unbalanced.
 */
struct lll_object *
lll_read(FILE * fd) {
        /* for parentheses counting. */
        uint32_t deep = 0;
        char preffix[255];
        int plength = 0;
        char c;
        length = 0;
        bool is_sexp = false;

        c = getc(fd);
        while (isspace(c)) {
                c = getc(fd);
        }
        int last_space = 0;

        while (c == '\'' || c == '`' || c == ',' || c == '@') {
                preffix[plength++] = c;
                c = getc(fd);
        }
        preffix[plength] = '\0';

        /* then we just read all chars before whitespace or EOF */
        if (c != '(') {
                if (c == '\"') {
                        do {
                                buffer[length++] = c;
                                if (length == BUFFER_SIZE - 1) {
                                        lll_error(14, "too big input", __FILE__, __LINE__);
                                }
                                c = getc(fd);
                        } while (c != '\"');
                        buffer[length++] = c;
                }
                else {
                        do {
                                buffer[length++] = c;
                                if (length == BUFFER_SIZE - 1) {
                                        lll_error(14, "too big input", __FILE__, __LINE__);
                                }
                                c = getc(fd);
                        } while ((feof(fd) != 0) || !(last_space = isspace(c)));
                }
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
                unparent(&strrec, strlen(strrec));
                return parse_sexp(strrec, preffix);
        }
        else {
                return parse_atom(buffer, preffix);
        }
}

struct lll_object *
parse_atom(char *str, char *preffix) {
        /* preffix contain ,'`,@ stuff */

        if (preffix[0] != '\0') {
                switch (preffix[0]) {
                  case '\'':
                          return lll_quote(parse_atom(str, &preffix[1]));
                          break;

                  case '`':
                          return lll_quasiquote(parse_atom(str, &preffix[1]));
                          break;

                  case ',':
                          if (preffix[1] == '@') {
                                  return lll_unquote_splicing(parse_atom(str, &preffix[2]));
                          }
                          return lll_unquote(parse_atom(str, &preffix[1]));
                          break;

                  default:
                          lll_error(14, "Unknown preffix", __FILE__, __LINE__);
                          return NULL;
                }
        }

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
                printf("[error]: '%s'\n", str); // to del
                lll_error(14, "Couldn't parse string", __FILE__, __LINE__);
        }

        return NULL;
}


/* makes " ( s y (z g) ((z) y) ) " to "s y (z g) ((z) y)" */
void
unparent(char **str, int length) {
        extern void pass_whitespace(char **);
        int i = length - 1;

        while (true) {
                if ((*str)[i] == ')') {
                        (*str)[i] = '\0';
                        i--;
                        break;
                }
                (*str)[i] = '\0';
                i--;
        }

        while (isspace((*str)[i])) {
                (*str)[i] = '\0';
                i--;
        }

        /* pass '(' */
        (*str)++;

        pass_whitespace(str);
}


int
sexp_length(char *str) {
        int result = 0;
        if (str[0] != '(') {
                lll_fatal_error(14,
                                "expected '(' as first char in sexp_length", __FILE__, __LINE__);
        }
        result++;
        int deep = 1;
        while (deep != 0) {
                if (str[result] == '(') {
                        deep++;
                }
                else if (str[result] == ')') {
                        deep--;
                }
                result++;
        }
        return result;
}


/* parse s-expression without parentheses */
struct lll_object *
parse_sexp(char *str, char *pref) {
        struct lll_object *result = NULL;

        if (pref[0] != '\0') {
                switch (pref[0]) {
                  case '\'':
                          return lll_quote(parse_sexp(str, &pref[1]));
                          break;

                  case '`':
                          return lll_quasiquote(parse_sexp(str, &pref[1]));
                          break;

                  case ',':
                          if (pref[1] == '@') {
                                  return lll_unquote_splicing(parse_sexp(str, &pref[2]));
                          }
                          return lll_unquote(parse_sexp(str, &pref[1]));
                          break;

                  default:
                          lll_error(14, "Unknown preffix", __FILE__, __LINE__);
                          return NULL;
                }
        }

        if (str[0] == '\0') {
                if (pref[0] != '\0') {
                        lll_error(14, "found whitespace after preffix", __FILE__, __LINE__);
                }
                return NULL;
        }

        pass_whitespace(&str);
        int n = 0;
        char *tmpstr = NULL;
        bool done = false;
        bool dotted = false;
        char preffix[255];
        int plength = 0;
        preffix[0] = '\0';
        struct lll_object *tmp = NULL;
        while (str[0] != '\0') {
                if (str[0] == '\'' || str[0] == '`' || str[0] == ',' || str[0] == '@') {
                        preffix[plength++] = str[0];
                        str = &str[1];
                        continue;
                }
                else {
                        preffix[plength] = '\0';
                }

                if (plength != 0 && isspace(str[0])) {
                        lll_error(14, "found whitespace after preffix", __FILE__, __LINE__);
                }

                if (isspace(str[n]) || str[n] == '\"' || str[n] == '\0') {
                        if (str[n] == '\0') {
                                done = true;
                        }
                        str[n] = '\0';

                        /* dot (* . ?) case */
                        if (str[0] == '.' && str[1] == '\0') {
                                dotted = true;
                                str = &str[n + 1];
                                n = 0;
                                pass_whitespace(&str);
                                continue;
                        }

                        tmp = parse_atom(str, preffix);
                        plength = 0;

                        if (dotted) {
                                lll_append_as_last_cdr(&result, tmp);
                                str = &str[n + 1];
                                pass_whitespace(&str);
                                if (str[0] != '\0') {
                                        printf("0:'%s'\n", str);
                                        lll_error(14,
                                                  "only one expression can be placed after '.'",
                                                  __FILE__, __LINE__);
                                }
                                return result;
                        }
                        else {
                                lll_append_to_list(&result, tmp);
                        }
                        if (done) {
                                break;
                        }
                        str = &str[n + 1];
                        pass_whitespace(&str);
                        n = 0;
                }
                else if (str[n] == '(') {

                        /* if was some atom before '(' */
                        if (n != 0) {
                                str[n] = '\0';

                                tmp = parse_atom(str, preffix);
                                plength = 0;
                                lll_append_to_list(&result, tmp);
                                str[n] = '(';
                        }
                        str = &str[n];
                        int length = sexp_length(str);
                        tmpstr = &str[length];
                        unparent(&str, length);
                        tmp = parse_sexp(str, preffix);
                        plength = 0;
                        if (dotted) {
                                lll_append_as_last_cdr(&result, tmp);
                                pass_whitespace(&tmpstr);
                                if (tmpstr[0] != '\0') {
                                        lll_error(14,
                                                  "only one expression can be placed after '.'",
                                                  __FILE__, __LINE__);
                                        return NULL;
                                }
                                return result;
                        }
                        else {
                                lll_append_to_list(&result, tmp);
                        }
                        str = tmpstr;
                        pass_whitespace(&str);
                        n = 0;
                }
                else {
                        n++;
                }
        }

        if (dotted) {
                lll_error(14, "expected expr after '.'", __FILE__, __LINE__);
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
