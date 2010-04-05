#ifndef LLL_SYMBOL_TABLE_HEADER
# define LLL_SYMBOL_TABLE_HEADER

# include <stdint.h>

# include "lll_types.h"

# define LLL_MAX_SYMBOL_LENGTH 60

# define LLL_ALLOWED_S         "!$%^&?*-+<>=/~"
# define LLL_NOT_ALLOWED_S     ",.:#'`@()[]{}\\"


struct lll_symbol_entry {
    /* here stores symbol-string */
    const char *string;
    /* Symbol contains binded values and string */
    struct lll_object *symbol;
    /* Pointer to entry, with same hashcode, but different string */
    struct lll_symbol_entry *another_entry;
};

bool isallowd(char);
void lll_init_symbol_table(void);
struct lll_object *lll_get_binded_object(const char *);
struct lll_object *lll_get_symbol(const char *);
void lll_bind_symbol(struct lll_object *, struct lll_object *);
void lll_bind_object(const char *, struct lll_object *);
void lll_unbind_symbol(struct lll_object *);
bool lll_correct_symbol_string_p(const char *);
void lll_print_hash_table(void);

#endif
