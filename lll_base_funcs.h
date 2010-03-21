#ifndef LLL_STD_FUNCTIONS_HEADER
# define LLL_STD_FUNCTIONS_HEADER

# include <stdio.h>
# include <stdbool.h>

# include "lll_types.h"

bool lll_nil_p(struct lll_object *);

struct lll_object *lll_cons(struct lll_object *, struct lll_object *arg2);
struct lll_object *lll_car(struct lll_object *);
struct lll_object *lll_cdr(struct lll_object *);

void lll_append_to_list(struct lll_object **, struct lll_object *);
void lll_append_as_last_cdr(struct lll_object **, struct lll_object *);
struct lll_object *lll_call_bf(struct lll_object *, struct lll_object *arg_list);
void lll_bind_base_constants(void);
void lll_bind_base_functions(void);

uint32_t lll_list_length(struct lll_object *list);

#endif
