#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "lll_types.h"
#include "lll_symtable.h"
#include "lll_base_funcs.h"
#include "lll_utils.h"

static struct lll_symbol_entry hash_table[UINT16_MAX + 1];      /* from 0..UINT16_MAX */

/* That is CRC-16 CCITT. I just copy-paste it from wikipedia %). */
uint16_t
hash_func(const char *data) {
        uint16_t result = 0xFFFF;

        for (uint16_t i = 0; data[i] != '\0'; ++i) {
                result ^= (data[i] << 8);
                for (uint8_t j = 0; j < 8; ++j) {
                        /* if left bit is 1 */
                        if (result & 0x8000) {
                                result = (result << 1) ^ 0x1021;
                        }
                        else {
                                result <<= 1;
                        }
                }
        }
        return result;
}


bool
isallowd(char c) {
        return ((strchr(LLL_ALLOWED_S, c) != NULL) || isdigit(c) || isalnum(c));
}


/* Check is symbol_string correct.
   First char can't be digit.
   chars ()[]{}'.,`:@#;\ not allowed. see LLL_ALLOWED_S, LLL_NOT_ALLOWED_S
   But chars !$%^&?*-+<>/=~ correct.
   Also, string can't contain whitespace.
*/
bool
lll_correct_symbol_string_p(const char *symbol_string) {
        int length = strlen(symbol_string);
        if (length > LLL_MAX_SYMBOL_LENGTH) {
                lll_error(0, symbol_string, __FILE__, __LINE__);
        }

        if (length == 0) {
                lll_error(1, symbol_string, __FILE__, __LINE__);
        }

        if ((isdigit(symbol_string[0]) == 0) && (isallowd(symbol_string[0]) != 0)) {
                for (int i = 1; i < length; ++i) {
                        if (!isspace(symbol_string[i]) && isallowd(symbol_string[i])) {
                                continue;       /* all is ok. go forward. */
                        }
                        else {
                                return false;
                        }
                }
        }
        else {
                return false;
        }
        return true;
}


void
lll_init_symbol_table() {
        for (uint32_t i = 0; i <= UINT16_MAX; ++i) {
                hash_table[i].symbol = NULL;
                hash_table[i].another_string = NULL;
        }
}


struct lll_object *
lll_get_binded_object(const char *symbol_string) {
        struct lll_object *obj = lll_get_symbol(symbol_string);
        return obj->d.symbol->pair.car;
}


struct lll_object *
lll_get_symbol(const char *symbol_string) {
        if (!lll_correct_symbol_string_p(symbol_string)) {
                lll_error(2, symbol_string, __FILE__, __LINE__);
                return NULL;
        }

        /* here allocates mem for string, if need.
           if hashtable contain entry with this string,
           we must free this mem.
         */
        bool allocated = false;
        char *lowercase_version = lll_to_lowercase(symbol_string, &allocated);

        struct lll_symbol_entry *entry = &hash_table[hash_func(lowercase_version)];

        while (true) {
                /* haven't binded symbols. */
                if (entry == NULL) {
                        struct lll_object *obj = lll_csymbol(lowercase_version);
                        obj->d.symbol->pair.car = LLL_UNDEFINED;

                        entry = MALLOC_STRUCT(lll_symbol_entry);
                        entry->symbol = obj;
                        entry->another_string = NULL;

                        return obj;
                }

                if (entry->symbol == NULL) {
                        struct lll_object *obj = lll_csymbol(lowercase_version);
                        obj->d.symbol->pair.car = LLL_UNDEFINED;
                        entry->symbol = obj;

                        return obj;
                }

                if (strcmp(entry->symbol->d.symbol->string, lowercase_version) == 0) {
                        if (allocated) {
                                free((void *) lowercase_version);
                        }
                        return entry->symbol;
                }
                entry = entry->another_string;
        }
}


void
lll_bind_symbol(const char *symbol_string, struct lll_object *obj) {
        if (!lll_correct_symbol_string_p(symbol_string)) {
                lll_error(2, symbol_string, __FILE__, __LINE__);
        }

        /* here allocates mem for string, if need.
           if hashtable contain entry with this string,
           we must free this mem.
         */
        bool allocated = false;
        char *lowercase_version = lll_to_lowercase(symbol_string, &allocated);

        struct lll_symbol_entry *entry = &hash_table[hash_func(lowercase_version)];
        /* Works endless if in some entry
           ahother_string will be not initialized with NULL */
        while (true) {
                if (entry == NULL) {
                        entry = MALLOC_STRUCT(lll_symbol_entry);
                        entry->symbol = NULL;
                        entry->another_string = NULL;
                }

                if (entry->symbol == NULL) {
                        /* If symbol undefined, just bind it. */
                        struct lll_object *sobj = lll_csymbol(lowercase_version);
                        sobj->d.symbol->pair.car = obj;
                        sobj->d.symbol->pair.cdr = NULL;
                        entry->symbol = sobj;
                        return;
                }

                /* If this symbol binded, then
                   push on top obj value.
                   Address of top pair don't changed.
                 */
                if (strcmp(entry->symbol->d.symbol->string, lowercase_version) == 0) {
                        if (allocated) {
                                free((void *) lowercase_version);
                        }

                        struct lll_pair *top_pair = &entry->symbol->d.symbol->pair;
                        struct lll_object *new_pair = MALLOC_STRUCT(lll_object);
                        new_pair->type_code = LLL_PAIR;

                        new_pair->d.pair->car = top_pair->car;
                        new_pair->d.pair->cdr = top_pair->cdr;
                        top_pair->cdr = new_pair;
                        top_pair->car = obj;

                        return;
                }

                /* if strings different we must search another. */
                entry = entry->another_string;
        }
}
