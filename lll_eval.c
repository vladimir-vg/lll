#include "lll_types.h"
#include "lll_utils.h"
#include "lll_base_funcs.h"
#include "lll_eval.h"

struct lll_object *
lll_eval(struct lll_object *obj) {
        if (obj == NULL) {
                return NULL;
        }

        /* if we get an s-expression */
        if ((obj->type_code & LLL_PAIR) != 0) {
                return lll_call_bf(obj->d.pair->car,
                                   obj->d.pair->cdr->d.pair);
        }
        /* if we get an atom */
        else {
                switch (obj->type_code) {
                case LLL_CHAR:
                        return obj;
                        break;

                case LLL_SYMBOL:
                        return obj->d.symbol->pair.car;
                        break;

                case LLL_STRING:
                        return obj;
                        break;

                case LLL_INTEGER32:
                        return obj;
                        break;

                case LLL_LAMBDA:
                        return obj;
                        break;

                case LLL_BUILTIN_FUNCTION:
                        return obj;
                        break;

                default:
                        lll_fatal_error(6, "eval");
                        return NULL;
                        break;
                }
        }
}
