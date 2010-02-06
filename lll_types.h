#ifndef LLL_TYPES_HEADER
# define LLL_TYPES_HEADER

# include <stdint.h>
# include <stdbool.h>

/* Type code. Representes what kind of type we have. [_][_|_|_] */
/* Some characteristic. One byte. */
# define LLL_CONST                       0x01000000
# define LLL_QUOTED                      0x02000000     /* '(some list) */
# define LLL_QUASIQUOTED                 0x04000000     /* `(some list) */
# define LLL_UNQUOTED                    0x08000000     /* ,(some list) */

/* Type. Takes three bytes. */
# define LLL_PAIR                        0x00000001
# define LLL_SYMBOL                      0x00000002
# define LLL_CHAR                        0x00000004
# define LLL_STRING                      0x00000008
# define LLL_INTEGER32                   0x00000010     /* always signed */
# define LLL_LAMBDA                      0x00000020
# define LLL_BUILTIN_FUNCTION            0x00000040

# define LLL_EXTERNAL_TYPE               0x00800000

/* Some constaint pointers like NULL
   I use them only for lll_object.
 */
# define LLL_T         ((void *) ((int) NULL + 1))
# define LLL_UNDEFINED ((void *) ((int) NULL + 2))

/* Anything lisp value representes with this structure. */
struct lll_object {
        uint32_t type_code;
        /* here for exten quoting: bool ex; */
        union {
                void *data;
                struct lll_object *obj; /* need's if we work with quotes */
                struct lll_pair *pair;
                struct lll_symbol *symbol;
                struct lll_builtin_function *bf;
                char c;         /* for LLL_CHAR type */
                const char *string;     /* for LLL_STRING type */
                int32_t integer32;      /* for LLL_INTEGER32 type */
        } d;
};


/* Pair structure. Cons cell. */
struct lll_pair {
        struct lll_object *car;
        struct lll_object *cdr;
};


struct lll_symbol {
        const char *string;
        /* car -- last binded values.
           cdr -- previous.
           if car == NULL, then this symbol not binded.
         */
        struct lll_pair pair;
};

/* Type of function that takes list and returns lisp object. */
typedef struct lll_object *(*lll_lisp_func) (struct lll_pair *);

struct lll_builtin_function {
        lll_lisp_func function;
        uint32_t args_count;
        bool last_as_rest;
};

/* Construction funcs. Have lll_c<type> view. */
struct lll_object *lll_cchar(char);
struct lll_object *lll_cstring(const char *);
struct lll_object *lll_cinteger32(int32_t);
struct lll_object *lll_csymbol(const char *);

#endif
