#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "lll_types.h"
#include "lll_print.h"
#include "lll_symtable.h"
#include "lll_base_funcs.h"
#include "lll_utils.h"

/* from 0..UINT16_MAX */
static struct lll_symbol_entry hash_table[UINT16_MAX + 1];

uint16_t hash_func(const char *data);

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
lll_init_symbol_table(void) {
    for (uint32_t i = 0; i <= UINT16_MAX; ++i) {
        hash_table[i].symbol = NULL;
        hash_table[i].another_entry = NULL;
        hash_table[i].string = NULL;
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

    char *lowercase_version = lll_to_lowercase(symbol_string);

    if (strcmp(lowercase_version, "nil") == 0) {
        free(lowercase_version);
        return NULL;
    }

    struct lll_symbol_entry *entry = &hash_table[hash_func(lowercase_version)];

    while (true) {
        /* haven't binded symbols. */
        if (entry == NULL) {
            entry = MALLOC_STRUCT(lll_symbol_entry);
            entry->string = lowercase_version;

            struct lll_object *obj = lll_csymbol(entry->string);
            obj->d.symbol->pair.car = LLL_UNDEFINED;

            entry->symbol = obj;
            entry->another_entry = NULL;

            return entry->symbol;
        }

        if (entry->string == NULL) {
            entry->string = lowercase_version;
            entry->symbol = lll_csymbol(entry->string);
            entry->symbol->d.symbol->pair.car = LLL_UNDEFINED;

            return entry->symbol;
        }

        if (strcmp(entry->string, lowercase_version) == 0) {
            /* So we have one copy in hash_table. Free second. */
            free(lowercase_version);

            /* string not binded yet */
            if (entry->symbol == NULL) {
                entry->symbol = lll_csymbol(entry->string);
                entry->symbol->d.symbol->pair.car = LLL_UNDEFINED;

                return entry->symbol;
            }
            /* string binded */
            else {
                return entry->symbol;
            }
        }
        entry = entry->another_entry;
    }
}


void
lll_bind_symbol(struct lll_object *sym, struct lll_object *obj) {
    if ((sym->type_code & LLL_SYMBOL) == 0) {
        lll_error(20, NULL, __FILE__, __LINE__);
    }

    struct lll_pair *top_pair = &sym->d.symbol->pair;
    struct lll_object *new_pair = MALLOC_STRUCT(lll_object);
    new_pair->type_code = LLL_PAIR;
    new_pair->d.pair = MALLOC_STRUCT(lll_pair);

    new_pair->d.pair->car = top_pair->car;
    new_pair->d.pair->cdr = top_pair->cdr;
    top_pair->car = obj;
    top_pair->cdr = new_pair;

    return;
}


void
lll_bind_object(const char *symbol_string, struct lll_object *obj) {
    if (!lll_correct_symbol_string_p(symbol_string)) {
        lll_error(2, symbol_string, __FILE__, __LINE__);
    }

    char *lowercase_version = lll_to_lowercase(symbol_string);

    struct lll_symbol_entry *entry = &hash_table[hash_func(lowercase_version)];
    /* Works endless if in some entry
       ahother_entry will be not initialized with NULL */
    while (true) {
        if (entry == NULL) {
            entry = MALLOC_STRUCT(lll_symbol_entry);
            entry->string = lowercase_version;
            entry->symbol = lll_csymbol(entry->string);
            entry->symbol->d.symbol->pair.car = obj;
            entry->another_entry = NULL;

            return;
        }

        /* first entry */
        if (entry->string == NULL) {
            entry->string = lowercase_version;
            entry->symbol = lll_csymbol(entry->string);
            entry->symbol->d.symbol->pair.car = obj;
            entry->another_entry = NULL;

            return;
        }

        if (strcmp(entry->string, lowercase_version) == 0) {
            free(lowercase_version);
            lll_bind_symbol(entry->symbol, obj);

            return;
        }

        /* if strings different we must search another. */
        entry = entry->another_entry;
    }
}

/* takes symbol */
void
lll_unbind_symbol(struct lll_object *sym) {
    struct lll_pair *p = &sym->d.symbol->pair;

    /* here leak. later must be fixed. */
    sym->d.symbol->pair.car = p->cdr->d.pair->car;
    sym->d.symbol->pair.cdr = p->cdr->d.pair->cdr;
}

void
lll_print_hash_table() {
    for (uint16_t i = 0; i < UINT16_MAX; ++i) {
        if (hash_table[i].string != NULL) {
            printf("(%d\t('%s': ", i, hash_table[i].string);

            struct lll_pair *p = &hash_table[i].symbol->d.symbol->pair;
            while (p != NULL) {
                lll_display(p->car);
                if (p->cdr == NULL) {
                    break;
                }
                else {
                    p = p->cdr->d.pair;
                }
                printf(" -> ");
            }

            printf(")\n");

            struct lll_symbol_entry *entry = hash_table[i].another_entry;
            while (entry != NULL) {
                printf("\t('%s': ", entry->string);

                struct lll_pair *p = &entry->symbol->d.symbol->pair;
                while (p != NULL) {
                    lll_display(p->car);
                    printf(" ");
                    if (p->cdr == NULL) {
                        break;
                    }
                    else {
                        p = p->cdr->d.pair;
                    }
                }

                printf(")\n");
            }
            printf("\t)\n\n");
        }
    }
}
