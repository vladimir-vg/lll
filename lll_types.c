#include <stdlib.h>

#include "lll_utils.h"
#include "lll_types.h"
#include "lll_symtable.h"

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


/* don't use that function. It just construct symbol object without any checking. */
struct lll_object *
lll_csymbol(const char *symbol_string) {
        if (symbol_string == NULL) {
                lll_fatal_error(9, "symbol_string'' in ``csymbol");
        }

        struct lll_object *symbol = MALLOC_STRUCT(lll_object);
        symbol->d.symbol = MALLOC_STRUCT(lll_symbol);
        symbol->type_code = LLL_SYMBOL;
        //symbol->d.obj = MALLOC_STRUCT(lll_object);
        symbol->d.symbol->pair.car = NULL;
        symbol->d.symbol->pair.cdr = NULL;
        symbol->d.symbol->symbol_string = symbol_string;
        return symbol;
}
