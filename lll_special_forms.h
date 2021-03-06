#ifndef LLL_SPECIAL_FORMS_HEADER
# define LLL_SPECIAL_FORMS_HEADER

# include "lll_types.h"

struct lll_object *lll_and   (struct lll_object *);
struct lll_object *lll_or    (struct lll_object *);
struct lll_object *lll_define(struct lll_object *);
struct lll_object *lll_if    (struct lll_object *);
struct lll_object *lll_load  (struct lll_object *);

#endif
