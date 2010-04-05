#ifndef LLL_READ_HEADER
# define LLL_READ_HEADER

# include <stdio.h>

# include "lll_types.h"

# define LLL_MAX_INPUT_SIZE 4096

struct lll_object *lll_read(FILE *);

#endif
