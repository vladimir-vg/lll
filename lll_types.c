#include <stdlib.h>
#include <stdio.h>

#include "lll_utils.h"
#include "lll_types.h"
#include "lll_symtable.h"

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
    obj->d.symbol->pair.car = NULL;
    obj->d.symbol->pair.cdr = NULL;
    obj->d.symbol->string = symbol_string;
    return obj;
}
