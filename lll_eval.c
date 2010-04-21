#include <stdint.h>
#include <string.h>

#include "lll_types.h"
#include "lll_utils.h"
#include "lll_base_funcs.h"
#include "lll_symtable.h"
#include "lll_print.h"
#include "lll_eval.h"
#include "lll_special_forms.h"

struct lll_object *call_lambda(struct lll_object *, struct lll_object *);
struct lll_object *calculate_list(struct lll_object *);

struct lll_object *
lll_call_bf(struct lll_object *symbol, struct lll_object *arg_list) {
#ifdef LLL_DEBUG
    printf("call_bf: ");
    lll_display(symbol);
    printf(" ");
    lll_display(arg_list);
    printf("\n");
#endif
    if ((symbol->type_code & LLL_SYMBOL) == 0) {
        lll_display(symbol);
        lll_fatal_error(7, "call_bf", __FILE__, __LINE__);
    }

    struct lll_object *func = lll_car(symbol->d.symbol->pair);

    if ((func->type_code & LLL_BUILTIN_FUNCTION) == 0) {
        lll_fatal_error(8, symbol->d.symbol->string, __FILE__, __LINE__);
    }

    return (*func->d.bf->function) (arg_list);
}

struct lll_object *
call_lambda(struct lll_object *l, struct lll_object *args) {
    if (lll_list_length(args) != lll_list_length(l->d.lambda->args)) {
        lll_error(17, "lambda", __FILE__, __LINE__);
    }

    struct lll_object *largs = l->d.lambda->args;
    while (largs != NULL) {
        lll_bind_symbol(lll_car(largs), lll_car(args));

        largs = lll_cdr(largs);
        args = lll_cdr(args);
    }

    struct lll_object *lbody = l->d.lambda->body;
    struct lll_object *result = NULL;
    while (lbody != NULL) {
        result = lll_eval(lll_car(lbody));
        lbody = lll_cdr(lbody);
    }

    largs = l->d.lambda->args;
    while (largs != NULL) {
        lll_unbind_symbol(lll_car(largs));

        largs = lll_cdr(largs);
    }

    return result;
}

struct lll_object *
calculate_list(struct lll_object *args) {
    struct lll_object *result = NULL;
    struct lll_object *tmp = NULL;
    while (args != NULL) {
        tmp = lll_eval(lll_car(args));
        lll_append_to_list(&result, tmp);
        args = lll_cdr(args);
    }
    return result;
}

struct lll_object *
lll_eval(struct lll_object *obj) {
    if (obj == NULL) {
        return NULL;
    }

    if (obj == LLL_UNDEFINED()) {
        lll_error(23, "get undefined value", __FILE__, __LINE__);
    }

    if (obj == LLL_VOID()) {
        lll_error(23, "get void value", __FILE__, __LINE__);
    }


    if (lll_integer32_p(obj) != NULL || lll_string_p(obj) != NULL || lll_char_p(obj) != NULL) {
        return obj;
    }
    else if (lll_quote_p(obj) != NULL) {
        return obj->d.obj;
    }
    else if (lll_symbol_p(obj) != NULL) {
        struct lll_symbol *sym = obj->d.symbol;
        struct lll_object *car = lll_car(sym->pair);

        if (car == LLL_UNDEFINED()) {
            lll_error(23, "symbol has undefined value", __FILE__, __LINE__);
        }
        else if (car == NULL) {
            return NULL;
        }
        else {
            return car;
        }
    }
    else if (lll_pair_p(obj) != NULL) {
        struct lll_object *car = lll_car(obj);
        struct lll_object *cdr = lll_cdr(obj);

        /* special forms */
        if (car == lll_get_symbol("and")) {
            return lll_and(cdr);
        }
        else if (car == lll_get_symbol("or")) {
            return lll_or(cdr);
        }
        else if (car == lll_get_symbol("lambda")) {
            return lll_clambda(lll_car(cdr), lll_cdr(cdr));
        }

        if (lll_pair_p(car) != NULL) {
            car = lll_eval(car);
        }

        if (lll_symbol_p(car) != NULL) {
            struct lll_object *binded = lll_car(car->d.symbol->pair);
            if (binded == NULL || binded == LLL_UNDEFINED() ||
                (lll_bf_p(binded) == NULL && lll_lambda_p(binded) == NULL)
                ) {
                lll_error(12, NULL, __FILE__, __LINE__);
            }

            int32_t l = lll_list_length(cdr);
            if (binded->d.bf->args_count != l) {
                lll_error(17, "call_bf", __FILE__, __LINE__);
            }
            return lll_call_bf(car, calculate_list(cdr));
        }
        else {
            if (car == NULL || car == LLL_UNDEFINED() || lll_lambda_p(car) == NULL) {
                lll_error(12, NULL, __FILE__, __LINE__);
            }
            else {
                return call_lambda(car, calculate_list(cdr));
            }
        }
    }
    return NULL;
}
