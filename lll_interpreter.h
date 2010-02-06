#ifndef LLL_INTERPRETER_HEADER
# define LLL_INTERPRETER_HEADER

# include <setjmp.h>

# include "lll_types.h"

extern jmp_buf env_buf;

extern struct lll_object *result;
extern struct lll_object *prev_result;

#endif
