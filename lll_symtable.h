#ifndef LLL_SYMBOL_TABLE_HEADER
# define LLL_SYMBOL_TABLE_HEADER

# include <stdint.h>

# include "lll_types.h"

# define LLL_MAX_SYMBOL_LENGTH 60

/* static const char LLL_ALLOWED_S[] = "!$%^&?*-+<>=/~"; */
# define LLL_NOT_ALLOWED_S ",.:#'`@()[]{}\\"


struct lll_symbol_entry {
        /* Symbol contains binded values and string */
        struct lll_object *symbol;
        /* Pointer to entry, with same hashcode, but different string. */
        struct lll_symbol_entry *another_string;
};


void lll_init_symbol_table();
struct lll_object *lll_get_binded_object(const char *);
struct lll_object *lll_get_symbol(const char *);
void lll_bind_symbol(const char *, struct lll_object *);

#endif
