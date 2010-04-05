#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "lll_types.h"
#include "lll_utils.h"
#include "lll_symtable.h"
#include "lll_base_funcs.h"
#include "lll_eval.h"
#include "lll_print.h"

struct lll_object *lll_plus(struct lll_object *, struct lll_object *);

struct lll_object *lll_bf_nil_p(struct lll_object *);
struct lll_object *lll_bf_not(struct lll_object *);
struct lll_object *lll_bf_pair_p(struct lll_object *);
struct lll_object *lll_bf_cons(struct lll_object *);
struct lll_object *lll_bf_list(struct lll_object *);
struct lll_object *lll_bf_length(struct lll_object *);
struct lll_object *lll_bf_car(struct lll_object *);
struct lll_object *lll_bf_cdr(struct lll_object *);
struct lll_object *lll_bf_eval(struct lll_object *);
struct lll_object *lll_bf_print(struct lll_object *);
struct lll_object *lll_bf_quit(struct lll_object *);
struct lll_object *lll_bf_plus(struct lll_object *);

void lll_bind_bf(lll_lisp_func, const char *, uint32_t);

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
lll_list_length(struct lll_object * obj) {
    if (obj == NULL) {
        return 0;
    }

    uint32_t result = 0;
    while (true) {
        result++;

        if (lll_cdr(obj) == NULL) {
            break;
        }

        if (lll_cdr(obj)->type_code == LLL_PAIR) {
            obj = lll_cdr(obj);
        }
        else {
            lll_error(5, "length", __FILE__, __LINE__);
        }
    }

    return result;
}

struct lll_object *
lll_car(struct lll_object *obj) {
    /* special case for NIL-symbol. It also empty list. */
    if (obj == NULL) {
        return NULL;            /* Just return NIL symbol. */
    }

    if ((obj->type_code & LLL_PAIR) == 0) {
        lll_error(9, "car", __FILE__, __LINE__);
        return NULL;
    }

    return obj->d.pair->car;
}

struct lll_object *
lll_cdr(struct lll_object *obj) {
    /* special case for NIL-symbol. It also empty list. */
    if (obj == NULL) {
        return NULL;            /* Just return NIL symbol. */
    }

    if ((obj->type_code & LLL_PAIR) == 0) {
        lll_error(9, "cdr", __FILE__, __LINE__);
        return NULL;
    }

    return obj->d.pair->cdr;
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
    while (lll_cdr(tmp) != NULL) {
        tmp = lll_cdr(tmp);
    }
    tmp->d.pair->cdr = obj;
}

struct lll_object *
lll_plus(struct lll_object *a, struct lll_object *b) {
    if ((a->type_code & LLL_INTEGER32) == 0 || (b->type_code & LLL_INTEGER32) == 0) {
        lll_error(16, "+", __FILE__, __LINE__);
    }
    return lll_cinteger32(a->d.integer32 + b->d.integer32);
}

/***************************************************/
/* builtin functions(bf) here: */
/* all that functions don't check number of arguments.
 */

struct lll_object *
lll_bf_nil_p(struct lll_object *arg_list) {
    if (lll_car(arg_list) == NULL) {
        return lll_get_symbol("t");
    }
    return NULL;
}

struct lll_object *
lll_bf_not(struct lll_object *arg_list) {
    if (lll_car(arg_list) == NULL) {
        return lll_get_symbol("t");
    }

    return NULL;
}

struct lll_object *
lll_bf_pair_p(struct lll_object *arg_list) {
    struct lll_object *car = lll_car(arg_list);
    if (car == NULL || car == LLL_VOID || car == LLL_UNDEFINED) {
        return NULL;
    }

    if ((car->type_code & LLL_PAIR) != 0) {
        return lll_get_symbol("t");
    }

    return NULL;
}

struct lll_object *
lll_bf_cons(struct lll_object *arg_list) {
    return lll_cons(lll_car(arg_list), lll_car(lll_cdr(arg_list)));
}

struct lll_object *
lll_bf_list(struct lll_object *arg_list) {
    return arg_list;
}

struct lll_object *
lll_bf_length(struct lll_object *arg_list) {
    return lll_cinteger32(lll_list_length(lll_car(arg_list)));
}

struct lll_object *
lll_bf_car(struct lll_object *arg_list) {
    if ((lll_car(arg_list)->type_code & LLL_PAIR) == 0) {
        lll_error(9, "car", __FILE__, __LINE__);
    }

    return lll_car(lll_car(arg_list));
}

struct lll_object *
lll_bf_cdr(struct lll_object *arg_list) {
    if ((lll_car(arg_list)->type_code & LLL_PAIR) == 0) {
        lll_error(9, "cdr", __FILE__, __LINE__);
    }

    return lll_car(lll_cdr(arg_list));
}

struct lll_object *
lll_bf_eval(struct lll_object *arg_list) {
    return lll_eval(lll_car(arg_list));
}

struct lll_object *
lll_bf_print(struct lll_object *arg_list) {
    if (arg_list == NULL) {
        return NULL;
    }

    while (arg_list != NULL) {
        lll_display(lll_car(arg_list));
        arg_list = lll_cdr(arg_list);
    }

    return LLL_VOID;
}

struct lll_object *
lll_bf_quit(struct lll_object *arg_list) {
    /* just ignore */
    arg_list = NULL;
    exit(0);
}

struct lll_object *
lll_bf_plus(struct lll_object *arg_list) {
    struct lll_object *result = lll_cinteger32(0);

    while (arg_list != NULL) {
        result = lll_plus(result, lll_car(arg_list));
        arg_list = lll_cdr(arg_list);
    }

    return result;
}

/***************************************************/

void
lll_bind_base_constants(void) {
    lll_bind_object("nil", NULL);
    lll_bind_object("t", NULL);
    lll_bind_object("t", lll_get_symbol("t"));
}

void
lll_bind_bf(lll_lisp_func f, const char *f_name, uint32_t arg_count) {
    struct lll_object *obj = MALLOC_STRUCT(lll_object);
    obj->type_code = LLL_BUILTIN_FUNCTION;
    obj->d.bf = MALLOC_STRUCT(lll_builtin_function);

    obj->d.bf->function = f;
    obj->d.bf->args_count = arg_count;
    lll_bind_object(f_name, obj);
}

void
lll_bind_base_functions(void) {
    lll_bind_bf(lll_bf_nil_p, "nil?", 1);
    lll_bind_bf(lll_bf_not, "not", 1);
    lll_bind_bf(lll_bf_pair_p, "pair?", 1);
    lll_bind_bf(lll_bf_cons, "cons", 2);
    lll_bind_bf(lll_bf_list, "list", -1);
    lll_bind_bf(lll_bf_length, "length", 1);
    lll_bind_bf(lll_bf_car, "car", 1);
    lll_bind_bf(lll_bf_cdr, "cdr", 1);
    lll_bind_bf(lll_bf_eval, "eval", 1);
    lll_bind_bf(lll_bf_print, "print", -1);
    lll_bind_bf(lll_bf_quit, "quit", 0);
    lll_bind_bf(lll_bf_plus, "+", -1);
}
