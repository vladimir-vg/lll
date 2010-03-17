#include <stddef.h>

#include "lll_types.h"
#include "lll_eval.h"
#include "lll_symtable.h"

#include "lll_special_forms.h"

struct lll_object *
lll_and(struct lll_object *args) {
        /* (and) without arguments return true */
        if (args == NULL) {
                return lll_get_symbol("t");
        }

        struct lll_pair *pair = args->d.pair;

        struct lll_object *last = NULL;
        while (true) {
                last = lll_eval(pair->car);
                if (last == NULL) {
                        return NULL;
                }

                if (pair->cdr != NULL) {
                        pair = pair->cdr->d.pair;
                }
                else {
                        break;
                }
        }
        return last;
}

struct lll_object *
lll_or(struct lll_object *args) {
        if (args == NULL) {
                return lll_get_symbol("t");
        }

        struct lll_pair *pair = args->d.pair;

        struct lll_object *tmp = NULL;
        while (true) {
                tmp = lll_eval(pair->car);
                if (tmp != NULL) {
                        return tmp;
                }

                if (pair->cdr != NULL) {
                        pair = pair->cdr->d.pair;
                }
                else {
                        break;
                }
        }
        return NULL;
}
