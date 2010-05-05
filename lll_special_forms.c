#include <string.h>
#include <stddef.h>

#include "lll_types.h"
#include "lll_eval.h"
#include "lll_symtable.h"
#include "lll_base_funcs.h"
#include "lll_utils.h"
#include "lll_print.h"
#include "lll_read.h"

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

struct lll_object *
lll_define(struct lll_object *args) {
  struct lll_object *car = lll_car(args);
  struct lll_object *cdr = lll_cdr(args);

  if (!lll_symbol_p(car)) {
    lll_error(20, NULL, __FILE__, __LINE__);
  }
  
  lll_bind_symbol(car, lll_eval(lll_car(cdr)));
  printf("'");
  lll_display(car);
  printf("' defined\n");

  return LLL_VOID();
}

struct lll_object *
lll_if(struct lll_object *args) {
  int l = lll_list_length(args);
  if (l == 2) {
    struct lll_object *e = lll_eval(lll_car(args));
    if (e) {
      return lll_eval(lll_car(lll_cdr(args)));
    }
    else {
      return LLL_VOID();
    }
  }
  else if (l == 3) {
    struct lll_object *test = lll_car(args);
    struct lll_object *then_exp = lll_car(lll_cdr(args));
    struct lll_object *else_exp = lll_car(lll_cdr(lll_cdr(args)));
    if (lll_eval(test)) {
      return lll_eval(then_exp);
    }
    else {
      return lll_eval(else_exp);
    }
  }
  
  lll_error(21, "if", __FILE__, __LINE__);
  return NULL;
}

struct lll_object *
lll_load(struct lll_object *args) {
  if (lll_list_length(args) != 1) {
    lll_error(17, "load", __FILE__, __LINE__);
  }

  struct lll_object *arg = lll_car(args);
  
  if (!lll_string_p(arg)) {
    lll_error(21, "load", __FILE__, __LINE__);
  }

  FILE *fin = fopen(arg->d.string, "r");
  if (fin == NULL) {
    lll_error(24, strerror(errno), __FILE__, __LINE__);
  }

  struct lll_object *result = NULL;
  while (!feof(fin)) {
    result = lll_read(fin);
    lll_display(lll_eval(result));
    printf("\n");
  }

  return LLL_VOID();
}
