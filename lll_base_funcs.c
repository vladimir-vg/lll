#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "lll_types.h"
#include "lll_utils.h"
#include "lll_symtable.h"
#include "lll_base_funcs.h"
#include "lll_print.h"

struct lll_object *lll_bf_nil_p(struct lll_pair *);
struct lll_object *lll_bf_cons(struct lll_pair *);
struct lll_object *lll_bf_length(struct lll_pair *);
struct lll_object *lll_bf_car(struct lll_pair *);
struct lll_object *lll_bf_cdr(struct lll_pair *);

struct lll_object *lll_bf_quit(struct lll_pair *);
void lll_bind_bf(lll_lisp_func, const char *, uint32_t, bool);

bool
lll_nil_p(struct lll_object *obj) {
        return (obj == NULL);
}

struct lll_object *
lll_cons(struct lll_object *arg1, struct lll_object *arg2) {
        struct lll_object *result = MALLOC_STRUCT(lll_object);
        result->d.pair = MALLOC_STRUCT(lll_pair);
        result->type_code = LLL_PAIR;

        result->d.pair->car = arg1;
        result->d.pair->cdr = arg2;

        return result;
}

uint32_t
lll_list_length(struct lll_pair * list) {
        if (list == NULL || list->car == NULL) {
                return 0;
        }

        uint32_t result = 1;
        while (list->cdr != NULL) {
                result++;
                if (list->cdr->type_code == LLL_PAIR) {
                        list = list->cdr->d.pair;
                }
                else {
                        lll_fatal_error(5, "length", __FILE__, __LINE__);
                }
        }

        return result;
}

struct lll_object *
lll_car(struct lll_pair *pair) {
        /* special case for NIL-symbol. It also empty list. */
        if (pair == NULL) {
                return NULL;    /* Just return NIL symbol. */
        }
        return pair->car;
}

struct lll_object *
lll_cdr(struct lll_pair *pair) {
        /* special case for NIL-symbol. It also empty list. */
        if (pair == NULL) {
                return NULL;    /* Just return NIL symbol. */
        }
        return pair->cdr;
}

void
lll_append_to_list(struct lll_object **list, struct lll_object *obj) {
        if (*list == NULL) {
                *list = MALLOC_STRUCT(lll_object);
                (*list)->type_code = LLL_PAIR;
                (*list)->d.pair = MALLOC_STRUCT(lll_pair);
                (*list)->d.pair->car = obj;
                (*list)->d.pair->cdr = NULL;
                return;
        }

        struct lll_object *tmp = (*list);
        while (tmp->d.pair->cdr != NULL) {
                tmp = tmp->d.pair->cdr;
                if ((tmp->type_code & LLL_PAIR) == 0) {
                        lll_error(5, "append", __FILE__, __LINE__);
                }
        }
        tmp->d.pair->cdr = MALLOC_STRUCT(lll_object);
        tmp->d.pair->cdr->type_code = LLL_PAIR;
        tmp->d.pair->cdr->d.pair = MALLOC_STRUCT(lll_pair);
        tmp->d.pair->cdr->d.pair->car = obj;
        tmp->d.pair->cdr->d.pair->cdr = NULL;
}

void
lll_append_as_last_cdr(struct lll_object **list, struct lll_object *obj) {
        struct lll_object *tmp = (*list);
        while (tmp->d.pair->cdr != NULL) {
                tmp = tmp->d.pair->cdr;
        }
        tmp->d.pair->cdr = obj;
}

struct lll_object *
lll_call_bf(struct lll_object *symbol, struct lll_pair *arg_list) {
        if ((symbol->type_code & LLL_SYMBOL) == 0) {
                lll_display(symbol);
                lll_fatal_error(7, "call_bf", __FILE__, __LINE__);
        }

        struct lll_object *func = symbol->d.symbol->pair.car;


        if ((func->type_code & LLL_BUILTIN_FUNCTION) == 0) {
                lll_fatal_error(8, symbol->d.symbol->string, __FILE__, __LINE__);
        }

        uint32_t l = lll_list_length(arg_list);
        if (l > func->d.bf->args_count) {
                lll_error(4, symbol->d.symbol->string, __FILE__, __LINE__);
        }
        else if (l < func->d.bf->args_count) {
                lll_error(3, symbol->d.symbol->string, __FILE__, __LINE__);
        }

        return (*func->d.bf->function) (arg_list);
}

/***************************************************/
/* builtin functions(bf) here: */
/* all that functions don't check number of arguments.
 */

struct lll_object *
lll_bf_nil_p(struct lll_pair *arg_list) {
        if (arg_list->car == NULL) {
                return LLL_T;
        }
        return NULL;
}

struct lll_object *
lll_bf_cons(struct lll_pair *arg_list) {
        return lll_cons(arg_list->car, arg_list->cdr->d.pair->car);
}

struct lll_object *
lll_bf_length(struct lll_pair *arg_list) {
        if ((arg_list->car->type_code & LLL_PAIR) == 0) {
                lll_error(9, "length", __FILE__, __LINE__);
        }

        return lll_cinteger32(lll_list_length(arg_list->car->d.pair));
}

struct lll_object *
lll_bf_car(struct lll_pair *arg_list) {
        if ((arg_list->car->type_code & LLL_PAIR) == 0) {
                lll_error(9, "car", __FILE__, __LINE__);
        }

        return arg_list->car->d.pair->car;
}

struct lll_object *
lll_bf_cdr(struct lll_pair *arg_list) {
        if ((arg_list->car->type_code & LLL_PAIR) == 0) {
                lll_error(9, "cdr", __FILE__, __LINE__);
        }

        return arg_list->car->d.pair->cdr;
}

struct lll_object *
lll_bf_quit(struct lll_pair *arg_list) {
        /* just ignore */
        arg_list = NULL;

        exit(0);
}

/***************************************************/

void
lll_bind_base_constants(void) {
        lll_bind_symbol("nil", NULL);
        lll_bind_symbol("t", LLL_T);
}

void
lll_bind_bf(lll_lisp_func f, const char *f_name, uint32_t arg_count, bool last_as_rest) {
        struct lll_object *obj = MALLOC_STRUCT(lll_object);
        obj->type_code = LLL_BUILTIN_FUNCTION;
        obj->d.bf = MALLOC_STRUCT(lll_builtin_function);

        obj->d.bf->function = f;
        obj->d.bf->args_count = arg_count;
        obj->d.bf->last_as_rest = last_as_rest;
        lll_bind_symbol(f_name, obj);
}

void
lll_bind_base_functions(void) {
        lll_bind_bf(lll_bf_nil_p, "nil?", 1, false);
        lll_bind_bf(lll_bf_cons, "cons", 2, false);
        lll_bind_bf(lll_bf_length, "length", 1, false);
        lll_bind_bf(lll_bf_car, "car", 1, false);
        lll_bind_bf(lll_bf_cdr, "cdr", 1, false);

        lll_bind_bf(lll_bf_quit, "quit", 0, false);
}
