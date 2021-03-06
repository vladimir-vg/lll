#ifndef LLL_TYPES_HEADER
# define LLL_TYPES_HEADER

# include <stdint.h>
# include <stdbool.h>

/* Type code. Representes what kind of type we have. [_][_|_|_] */
/* Some characteristic. One byte. */
# define LLL_CONST                       0x01000000

/* Type. Takes three bytes. */
# define LLL_PAIR                        0x00000001
# define LLL_SYMBOL                      0x00000002
# define LLL_CHAR                        0x00000004
# define LLL_STRING                      0x00000008
# define LLL_INTEGER32                   0x00000010     /* always signed */
# define LLL_LAMBDA                      0x00000020
# define LLL_BUILTIN_FUNCTION            0x00000040
# define LLL_MACRO                       0x00000080
# define LLL_QUOTE                       0x00000100     /* ' */
# define LLL_QUASIQUOTE                  0x00000200     /* ` */
# define LLL_UNQUOTE                     0x00000400     /* , */
# define LLL_UNQUOTE_SPLICING            0x00000800     /* ,@ */

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
        struct lll_lambda *lambda;
        char c;                 /* for LLL_CHAR type */
        const char *string;     /* for LLL_STRING type */
        int32_t integer32;      /* for LLL_INTEGER32 type */
    } d;
};


/* Some constaint pointers like NULL
 */
struct lll_object *LLL_VOID(void);
struct lll_object *LLL_UNDEFINED(void);


/* Pair structure. Cons cell. */
struct lll_pair {
    struct lll_object *car;
    struct lll_object *cdr;
};


struct lll_symbol {
    const char *string;
    /* car -- last binded values.
       cdr -- previous.
       if car == LLL_UNDEFINED(), then this symbol not binded.
     */
    struct lll_object *pair;
};

/* Type of function that takes list and returns lisp object. */
typedef struct lll_object *(*lll_lisp_func) (struct lll_object *);

struct lll_builtin_function {
    lll_lisp_func function;
    int32_t args_count;         /* -1 for any number */
};

struct lll_lambda {
    /* contain list of symbol args. */
    struct lll_object *args;

    /* contain what code need to eval. */
    struct lll_object *body;
};

struct lll_object *lll_pair_p(struct lll_object *);
struct lll_object *lll_symbol_p(struct lll_object *);
struct lll_object *lll_char_p(struct lll_object *);
struct lll_object *lll_string_p(struct lll_object *);
struct lll_object *lll_integer32_p(struct lll_object *);
struct lll_object *lll_lambda_p(struct lll_object *);
struct lll_object *lll_bf_p(struct lll_object *);
struct lll_object *lll_macro_p(struct lll_object *);
struct lll_object *lll_quote_p(struct lll_object *);
struct lll_object *lll_quasiquote_p(struct lll_object *);
struct lll_object *lll_unquote_p(struct lll_object *);
struct lll_object *lll_unquote_splicing_p(struct lll_object *);

struct lll_object *lll_quote(struct lll_object *);
struct lll_object *lll_quasiquote(struct lll_object *);
struct lll_object *lll_unquote(struct lll_object *);
struct lll_object *lll_unquote_splicing(struct lll_object *);

/* Construction funcs. Have lll_c<type> view. */
struct lll_object *lll_cchar(char);
struct lll_object *lll_cstring(const char *);
struct lll_object *lll_cinteger32(int32_t);
struct lll_object *lll_clambda(struct lll_object *, struct lll_object *);

/* that function only constructs symbol object.
   Constructed object haven't any binding.
   use lll_get_symbol instead.
 */
struct lll_object *lll_csymbol(const char *);

#endif
