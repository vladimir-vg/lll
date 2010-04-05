#include <stdint.h>
#include <string.h>

#include "lll_types.h"
#include "lll_utils.h"
#include "lll_base_funcs.h"
#include "lll_eval.h"
#include "lll_special_forms.h"

struct lll_object *
lll_eval(struct lll_object *obj) {
        if (obj == NULL) {
                return NULL;
        }

        /* if we get an s-expression */
        if ((obj->type_code & LLL_PAIR) != 0) {
                if ((lll_car(obj)->type_code & LLL_SYMBOL) == 0) {
                        lll_error(7, "eval", __FILE__, __LINE__);
                        return NULL;
                }

                /* check for special forms */
                const char *symbol_string = lll_car(obj)->d.symbol->string;
                if (strcmp(symbol_string, "and") == 0) {
                        return lll_and(lll_cdr(obj));
                }
                else if (strcmp(symbol_string, "or") == 0) {
                        return lll_or(obj->d.pair->cdr);
                }

                struct lll_object *binded = lll_car(obj)->d.symbol->pair.car;
                if (binded == LLL_UNDEFINED) {
                        lll_error(15, symbol_string, __FILE__, __LINE__);
                        return NULL;
                }

                uint32_t tp = binded->type_code;

                int32_t l = lll_list_length(lll_cdr(obj));

                switch (tp) {
                  case LLL_BUILTIN_FUNCTION:
                          if (binded->d.bf->args_count == -1) {
                                  break;
                          }

                          if (l > binded->d.bf->args_count) {
                                  lll_error(4, symbol_string, __FILE__, __LINE__);
                          }
                          else if (l < binded->d.bf->args_count) {
                                  lll_error(3, symbol_string, __FILE__, __LINE__);
                          }
                          break;

                  default:;
                }

                /* Here we calculate arguments */
                struct lll_object *calc_arg_list = NULL;
                struct lll_object *tmp = NULL;
                struct lll_object *arg_list = obj->d.pair->cdr;
                while (arg_list != NULL) {
                        tmp = lll_eval(arg_list->d.pair->car);
                        lll_append_to_list(&calc_arg_list, tmp);
                        arg_list = arg_list->d.pair->cdr;
                }

                if ((tp & LLL_BUILTIN_FUNCTION) != 0) {
                        if (calc_arg_list != NULL) {
                                return lll_call_bf(obj->d.pair->car, calc_arg_list);
                        }
                        else {
                                return lll_call_bf(obj->d.pair->car, NULL);
                        }
                }
                else if ((tp & LLL_LAMBDA) != 0) {
                        lll_error(11, "Evaluating lambda-forms", __FILE__, __LINE__);
                        return NULL;
                }
                else {
                        lll_error(12, NULL, __FILE__, __LINE__);
                        return NULL;
                }
        }
        /* if we get an atom */
        else {
                switch (obj->type_code) {
                  case LLL_SYMBOL:
                          return obj->d.symbol->pair.car;
                          break;

                  case LLL_QUOTE:
                          return obj->d.obj;
                          break;

                  case LLL_STRING:
                  case LLL_CHAR:
                  case LLL_INTEGER32:
                  case LLL_LAMBDA:
                  case LLL_BUILTIN_FUNCTION:
                          return obj;
                          break;

                  default:
                          lll_error(6, "eval", __FILE__, __LINE__);
                          return NULL;
                }
        }
}
