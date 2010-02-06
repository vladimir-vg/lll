#ifndef LLL_UTILS_HEADER
# define LLL_UTILS_HEADER

# include <stdlib.h>
# include <stdbool.h>
# include <stdint.h>

# define MALLOC_STRUCT(A) ((struct A *) malloc(sizeof (struct A)))

void lll_fatal_error(uint32_t, const char *);
void lll_error(uint32_t, const char *);
void lll_warning(const char *);
char *lll_to_lowercase(const char *, bool *);
char **lll_alloc_string(const char *);

#endif
