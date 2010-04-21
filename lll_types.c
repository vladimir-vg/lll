#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lll_utils.h"
#include "lll_types.h"
#include "lll_print.h"
#include "lll_symtable.h"
#include "lll_base_funcs.h"

struct lll_object *
LLL_VOID() {
    return (struct lll_object *) 2;
}

struct lll_object *
LLL_UNDEFINED() {
    return (struct lll_object *) 1;
}

struct lll_object *
lll_pair_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_PAIR) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_symbol_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_SYMBOL) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_char_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_CHAR) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_string_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_STRING) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_integer32_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_INTEGER32) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_lambda_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_LAMBDA) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_bf_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_BUILTIN_FUNCTION) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_macro_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_MACRO) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_quote_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_QUOTE) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_quasiquote_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_QUASIQUOTE) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_unquote_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_UNQUOTE) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_unquote_splicing_p(struct lll_object *obj) {
    return ((obj->type_code & LLL_UNQUOTE_SPLICING) == 0) ? NULL : lll_get_symbol("t");
}

struct lll_object *
lll_quote(struct lll_object *obj) {
    struct lll_object *result = MALLOC_STRUCT(lll_object);
    result->type_code = LLL_QUOTE;
    result->d.obj = obj;

    return result;
}

struct lll_object *
lll_quasiquote(struct lll_object *obj) {
    struct lll_object *result = MALLOC_STRUCT(lll_object);
    result->type_code = LLL_QUASIQUOTE;
    result->d.obj = obj;

    return result;
}

struct lll_object *
lll_unquote(struct lll_object *obj) {
    struct lll_object *result = MALLOC_STRUCT(lll_object);
    result->type_code = LLL_UNQUOTE;
    result->d.obj = obj;

    return result;
}

struct lll_object *
lll_unquote_splicing(struct lll_object *obj) {
    struct lll_object *result = MALLOC_STRUCT(lll_object);
    result->type_code = LLL_UNQUOTE_SPLICING;
    result->d.obj = obj;

    return result;
}

struct lll_object *
lll_cchar(char c) {
    struct lll_object *result = MALLOC_STRUCT(lll_object);

    result->type_code = LLL_CHAR;
    result->d.c = c;

    return result;
}


struct lll_object *
lll_cstring(const char *string) {
    struct lll_object *result = MALLOC_STRUCT(lll_object);

    result->type_code = LLL_STRING;
    result->d.string = string;

    return result;
}


struct lll_object *
lll_cinteger32(int32_t i) {
    struct lll_object *result = MALLOC_STRUCT(lll_object);

    result->type_code = LLL_INTEGER32;
    result->d.integer32 = i;

    return result;
}

struct lll_object *
lll_clambda(struct lll_object *args, struct lll_object *body) {
    struct lll_object *tmp = args;
    while (tmp != NULL) {
        if ((lll_car(tmp)->type_code & LLL_SYMBOL) == 0) {
            lll_error(19, NULL, __FILE__, __LINE__);
        }
        tmp = lll_cdr(tmp);
    }

    /* check for eq args */
    if (args != NULL) {
        struct lll_object *car = lll_car(args);
        struct lll_object *cdr = lll_cdr(args);
        while (cdr != NULL) {
            struct lll_object *lcdr = cdr;
            while (lcdr != NULL) {
                struct lll_object *lcar = lll_car(lcdr);
                if (strcmp(car->d.symbol->string, lcar->d.symbol->string) == 0) {
                    lll_error(18, NULL, __FILE__, __LINE__);
                }
                lcdr = lll_cdr(lcdr);
            }
            car = lll_car(cdr);
            cdr = lll_cdr(cdr);
        }
    }

    struct lll_object *result = MALLOC_STRUCT(lll_object);
    result->d.lambda = MALLOC_STRUCT(lll_lambda);
    result->type_code = LLL_LAMBDA;
    result->d.lambda->args = args;
    result->d.lambda->body = body;

    return result;
}


/* don't use that function. It just construct symbol object without any checking.
   Use lll_get_symbol in lll_symtable.h instead.
 */
struct lll_object *
lll_csymbol(const char *symbol_string) {
    if (symbol_string == NULL) {
        lll_fatal_error(9, "symbol_string'' in ``csymbol", __FILE__, __LINE__);
    }

    struct lll_object *obj = MALLOC_STRUCT(lll_object);
    obj->d.symbol = MALLOC_STRUCT(lll_symbol);
    obj->type_code = LLL_SYMBOL;
    obj->d.symbol->pair = lll_cons(LLL_UNDEFINED(), NULL);
    obj->d.symbol->string = symbol_string;
    return obj;
}
