#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "lll_types.h"
#include "lll_utils.h"
#include "lll_symtable.h"


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

/* Prints object as text to file. */
void
lll_displayf(FILE * fd, struct lll_object *obj) {
        void lll_displayf_sub(FILE *, struct lll_object *);

        lll_displayf_sub(fd, obj);
        fprintf(fd, "\n");
}

void
lll_displayf_sub(FILE * fd, struct lll_object *obj) {
        if (obj == NULL) {
                fprintf(fd, "nil");
                return;
        }

        if (obj == LLL_UNDEFINED) {
                fprintf(fd, "<undefined value>");
                return;
        }

        if (obj == LLL_T) {
                fprintf(fd, "t");
                return;
        }

        if (obj->type_code == LLL_PAIR) {
                struct lll_pair *pair = obj->d.pair;
                struct lll_object *p_cdr = (struct lll_object *) pair->cdr;
                fprintf(fd, "(");
                lll_displayf_sub(fd, pair->car);

                /* if we have not just cons-cell, but list then: */
                while ((p_cdr != NULL) && (p_cdr != LLL_T) && (p_cdr != LLL_UNDEFINED)
                       && ((p_cdr->type_code & LLL_PAIR) != 0)) {
                        struct lll_pair *p_cdr_pair = p_cdr->d.pair;
                        fprintf(fd, " ");
                        lll_displayf_sub(fd, p_cdr_pair->car);
                        p_cdr = p_cdr_pair->cdr;
                }

                if (p_cdr == NULL) {
                        fprintf(fd, ")");
                }
                else {
                        fprintf(fd, " . ");
                        lll_displayf_sub(fd, p_cdr);
                        fprintf(fd, ")");
                }
        }
        /* if not pair */
        else {
                switch (obj->type_code) {
                  case LLL_SYMBOL:
                          fprintf(fd, "%s", obj->d.symbol->string);
                          break;

                  case LLL_CHAR:
                          fprintf(fd, "#\\%c", (char) obj->d.c);
                          break;

                  case LLL_STRING:
                          fprintf(fd, "\"%s\"", obj->d.string);
                          break;

                  case LLL_INTEGER32:
                          fprintf(fd, "%d", obj->d.integer32);
                          break;

                  case LLL_LAMBDA:     /* TODO: printing name. */
                          fprintf(fd, "<lambda>");
                          break;

                  case LLL_BUILTIN_FUNCTION:
                          fprintf(fd, "<builtin function>");
                          break;

                  default:
                          printf("%x\n", obj->type_code);
                          lll_fatal_error(6, "display", __FILE__, __LINE__);
                }
        }
}

void
lll_display(struct lll_object *obj) {
        lll_displayf(stdout, obj);
}

/* FIXME: need correct cases when list is nil */
void
lll_append_to_list(struct lll_object *list, struct lll_object *obj) {
        if (list == NULL) {
                list = MALLOC_STRUCT(lll_object);
                list->type_code = LLL_PAIR;
                list->d.pair = MALLOC_STRUCT(lll_pair);
                list->d.pair->car = obj;
                list->d.pair->cdr = NULL;
                return;
        }

        if (list->d.pair->car == NULL) {
                list->d.pair->car = obj;
                return;
        }

        while (list->d.pair->cdr != NULL) {
                list = list->d.pair->cdr;
                if ((list->type_code & LLL_PAIR) == 0) {
                        lll_error(5, "append", __FILE__, __LINE__);
                }
        }
        list->d.pair->cdr = MALLOC_STRUCT(lll_object);
        list->d.pair->cdr->type_code = LLL_PAIR;
        list->d.pair->cdr->d.pair = MALLOC_STRUCT(lll_pair);
        list->d.pair->cdr->d.pair->car = obj;
        list->d.pair->cdr->d.pair->cdr = NULL;
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

/***************************************************/

void
lll_bind_base_constants() {
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
lll_bind_base_functions() {
        lll_bind_bf(lll_bf_nil_p, "nil?", 1, false);
        lll_bind_bf(lll_bf_cons, "cons", 2, false);
        lll_bind_bf(lll_bf_length, "length", 1, false);
        lll_bind_bf(lll_bf_car, "car", 1, false);
        lll_bind_bf(lll_bf_cdr, "cdr", 1, false);
}
