#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "lll_interpreter.h"
#include "lll_types.h"
#include "lll_symtable.h"
#include "lll_base_funcs.h"

#include "lll_read.h"
#include "lll_eval.h"
#include "lll_print.h"

#include "lll_utils.h"

struct lll_object *result;
struct lll_object *prev_result;

jmp_buf env_buf;


int
main(void) {
        result = NULL;
        prev_result = NULL;

        lll_init_symbol_table();
        lll_bind_base_constants();
        lll_bind_base_functions();


        /* repl */
        while (true) {
                if (!setjmp(env_buf)) {
                        printf("> ");
                        result = lll_read(stdin);
                        lll_display(lll_eval(result));
                }
        }

        return 0;
}
