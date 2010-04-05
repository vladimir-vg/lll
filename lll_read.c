#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lll_read.h"
#include "lll_symtable.h"
#include "lll_utils.h"
#include "lll_types.h"
#include "lll_base_funcs.h"

char *read_expr(FILE *);
struct lll_object *parse_expr(char *, struct lll_object **);
struct lll_object *tokenize_string(const char *);

bool correct_char_p(char *);
bool correct_integer_p(char *);
bool correct_string_p(char *);
bool correct_symbol_p(char *);
struct lll_object *parse_string(char *);

struct lll_object *
lll_read(FILE * fd) {
    struct lll_object *tl = tokenize_string(read_expr(fd));
    if (tl == NULL) {
        return NULL;
    }
    struct lll_object *tlcdr = lll_cdr(tl);
    char *str = (char *) lll_car(tl)->d.string;

    struct lll_object *result = parse_expr(str, &tlcdr);

    /* free token string */
    while (tl != NULL) {
        free((char *) lll_car(tl)->d.string);
        tl = lll_cdr(tl);
    }

    return result;
}

char *
read_expr(FILE * fd) {
    int paren_counter = 0;
    bool quote_counter = false;

    char *str = (char *) malloc(LLL_MAX_INPUT_SIZE);
    uint32_t length = 0;
    while (true) {
        char c = fgetc(fd);
        if (c == '"') {
            quote_counter = !quote_counter;
        }

        if (!quote_counter) {
            if (c == '(') {
                paren_counter++;
            }
            else if (c == ')') {
                paren_counter--;
            }
        }
        str[length++] = c;

        if ((paren_counter == 0 && !quote_counter && (feof(fd) != 0 || isspace(c))) || (feof(fd) != 0)
            ) {
            break;
        }
    }

    if (quote_counter || paren_counter != 0) {
        free(str);
        lll_error(14, "parens or quotes are unbalanced", __FILE__, __LINE__);
    }

    str[length] = '\0';

    return str;
}

struct lll_object *
tokenize_string(const char *str) {
    struct lll_object *result = NULL;

    char *buffer = (char *) malloc(LLL_MAX_SYMBOL_LENGTH + 1);
    uint8_t length = 0;

    /* for `',@ reading */
    bool ch = false;
    bool prefix_allowed = true;
    bool string_reading = false;
    while (true) {
        if (str[0] == '"') {
            string_reading = !string_reading;
        }

        /* Just read string while not get '"' */
        if (!string_reading) {

            if (str[0] == '\'' || str[0] == '`' || str[0] == ',' || str[0] == '@') {
                if (prefix_allowed) {
                    buffer[length++] = str[0];
                    str++;
                    continue;
                }
                else {
                    free(buffer);
                    lll_error(14, "chars ,'`@ not allowed here", __FILE__, __LINE__);
                }
            }
            else {
                ch = true;
                if (isspace(str[0]) || str[0] == '(' || str[0] == ')') {
                    if (str[0] == '(' || str[0] == ')') {
                        if (length == 0) {
                            buffer[length++] = str[0];
                            buffer[length] = '\0';
                            lll_append_to_list(&result, lll_cstring(buffer));
                            length = 0;
                            buffer = (char *)
                                malloc(LLL_MAX_SYMBOL_LENGTH + 1);
                            str++;
                            prefix_allowed = true;
                            continue;
                        }
                        else {
                            if (prefix_allowed) {
                                buffer[length++] = str[0];
                                buffer[length] = '\0';
                                lll_append_to_list(&result, lll_cstring(buffer));
                                length = 0;
                                buffer = (char *)
                                    malloc(LLL_MAX_SYMBOL_LENGTH + 1);
                                str++;
                                prefix_allowed = true;
                                continue;
                            }
                            else {
                                buffer[length] = '\0';
                                lll_append_to_list(&result, lll_cstring(buffer));

                                length = 0;
                                buffer = (char *) malloc(2);
                                buffer[0] = str[0];
                                buffer[1] = '\0';
                                lll_append_to_list(&result, lll_cstring(buffer));
                                buffer = (char *)
                                    malloc(LLL_MAX_SYMBOL_LENGTH + 1);
                                str++;
                                prefix_allowed = true;
                                continue;
                            }
                        }
                    }
                    else {
                        if (length == 0) {
                            str++;
                            prefix_allowed = true;
                            continue;
                        }
                        else {
                            buffer[length] = '\0';
                            lll_append_to_list(&result, lll_cstring(buffer));

                            length = 0;
                            buffer = (char *)
                                malloc(LLL_MAX_SYMBOL_LENGTH + 1);
                            str++;
                            prefix_allowed = true;
                            continue;
                        }
                    }
                }
                else if (str[0] == '\0') {
                    buffer[length] = '\0';
                    if (length != 0) {
                        lll_append_to_list(&result, lll_cstring(buffer));
                    }
                    /* here is the one point, where loop ends. */
                    break;
                }
            }
            if (ch) {
                ch = false;
                prefix_allowed = false;
            }

        }                       /* ends string reading. */

        buffer[length++] = str[0];
        str++;
    }

    return result;
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

struct lll_object *
parse_string(char *str) {
    if (str[0] == '\0') {
        lll_error(14, "get empty string as token", __FILE__, __LINE__);
    }

    if (correct_integer_p(str)) {
        int i;
        sscanf(str, "%d", &i);
        return lll_cinteger32(i);
    }
    else if (correct_char_p(str)) {
        return lll_cchar(str[2]);
    }
    else if (correct_string_p(str)) {
        int length = strlen(str);
        str[length - 1] = '\0';
        return lll_cstring(&str[1]);
    }
    else if (correct_symbol_p(str)) {
        return lll_get_symbol(str);
    }
    else {
        lll_error(14, "can't parse token", __FILE__, __LINE__);
    }

    /* will be never returned. Just for -Wall */
    return NULL;
}


struct lll_object *
parse_expr(char *head, struct lll_object **token_list) {
    switch (head[0]) {
      case '\0':
          lll_error(14, "got empty string as token", __FILE__, __LINE__);
          break;

      case '\'':
          return lll_quote(parse_expr(&head[1], token_list));

      case '`':
          return lll_quasiquote(parse_expr(&head[1], token_list));

      case ',':
          if (head[1] == '@') {
              return lll_unquote_splicing(parse_expr(&head[2], token_list));
          }
          return lll_unquote(parse_expr(&head[1], token_list));
      default:
          break;
    }

    struct lll_object *result = NULL;

    if (head[0] == '(') {
        if (head[1] != '\0') {
            lll_error(14, "found chars after (", __FILE__, __LINE__);
        }

        while (true) {
            char *str = (char *) lll_car(*token_list)->d.string;
            (*token_list) = lll_cdr(*token_list);

            if (str[0] == ')') {
                if (str[1] != '\0') {
                    lll_error(14, "found chars after )", __FILE__, __LINE__);
                }

                return result;
            }
            else {
                lll_append_to_list(&result, parse_expr(str, token_list));
            }

            if ((*token_list) == NULL) {
                lll_error(14, "unexpected end of token list. Probably parentheses unbalanced.", __FILE__, __LINE__);
            }
        }

    }
    else {                      /* Not pair */
        return parse_string(head);
    }
}
