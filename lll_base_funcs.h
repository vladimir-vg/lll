#ifndef LLL_STD_FUNCTIONS_HEADER
# define LLL_STD_FUNCTIONS_HEADER

# include <stdio.h>
# include <stdbool.h>

# include "lll_types.h"

bool lll_nil_p(struct lll_object *);

struct lll_object *lll_cons(struct lll_object *, struct lll_object *arg2);
struct lll_object *lll_car(struct lll_pair *);
struct lll_object *lll_cdr(struct lll_pair *);

void lll_displayf(FILE *, struct lll_object *);
void lll_display(struct lll_object *);
struct lll_object *lll_call_bf(struct lll_object *, struct lll_pair *arg_list);
void lll_bind_base_constants();
void lll_bind_base_functions();

#endif
