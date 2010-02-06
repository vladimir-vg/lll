#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "lll_interpreter.h"
#include "lll_types.h"
#include "lll_symtable.h"
#include "lll_base_funcs.h"
#include "lll_eval.h"
#include "lll_utils.h"

struct lll_object *result;
struct lll_object *prev_result;

jmp_buf env_buf;

#define C(X, Y) lll_cons(X, Y)
#define S(A)    lll_get_symbol(A)

int
main(int argc, char *argv[]) {
        result = NULL;
        prev_result = NULL;

        lll_init_symbol_table();
        lll_bind_base_constants();
        lll_bind_base_functions();

        /* repl begin */
        if (setjmp(env_buf)) {
        }
        else {
                struct lll_object *obj = C(S("cons"),
                                           C(S("A"),
                                             C(S("B"),
                                               NULL)));
                lll_display(obj);
                lll_display(lll_eval(obj));
        }
        /* repl ends */

        return 0;
}
