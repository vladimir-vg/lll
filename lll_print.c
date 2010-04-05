#include <stdio.h>

#include "lll_types.h"
#include "lll_print.h"
#include "lll_utils.h"


/* Prints object as text to file. */
void
lll_displayf(FILE * fd, struct lll_object *obj) {
    lll_displayf_sub(fd, obj);
}

void
lll_displayf_sub(FILE * fd, struct lll_object *obj) {
    if (obj == NULL) {
        fprintf(fd, "nil");
        return;
    }

    if (obj == LLL_VOID) {
        return;
    }

    if (obj == LLL_UNDEFINED) {
        fprintf(fd, "<undefined value>");
        return;
    }

    if (obj->type_code == LLL_PAIR) {
        struct lll_pair *pair = obj->d.pair;
        struct lll_object *p_cdr = (struct lll_object *) pair->cdr;
        fprintf(fd, "(");
        lll_displayf_sub(fd, pair->car);

        /* if we have not just cons-cell, but list then: */
        while ((p_cdr != NULL) && (p_cdr != LLL_UNDEFINED) && (p_cdr != LLL_VOID)
               && ((p_cdr->type_code & LLL_PAIR) != 0)) {
            struct lll_pair *p_cdr_pair = p_cdr->d.pair;
            fprintf(fd, " ");
            lll_displayf_sub(fd, p_cdr_pair->car);
            p_cdr = p_cdr_pair->cdr;
        }

        if (p_cdr == NULL) {
            fprintf(fd, ")");
        }
        else {
            fprintf(fd, " . ");
            lll_displayf_sub(fd, p_cdr);
            fprintf(fd, ")");
        }
    }
    /* if not pair */
    else {
        switch (obj->type_code) {
          case LLL_QUOTE:
              fprintf(fd, "'");
              lll_displayf_sub(fd, obj->d.obj);
              break;

          case LLL_QUASIQUOTE:
              fprintf(fd, "`");
              lll_displayf_sub(fd, obj->d.obj);
              break;

          case LLL_UNQUOTE:
              fprintf(fd, ",");
              lll_displayf_sub(fd, obj->d.obj);
              break;

          case LLL_UNQUOTE_SPLICING:
              fprintf(fd, ",@");
              lll_displayf_sub(fd, obj->d.obj);
              break;

          case LLL_SYMBOL:
              fprintf(fd, "%s", obj->d.symbol->string);
              break;

          case LLL_CHAR:
              fprintf(fd, "#\\%c", (char) obj->d.c);
              break;

          case LLL_STRING:
              fprintf(fd, "\"%s\"", obj->d.string);
              break;

          case LLL_INTEGER32:
              fprintf(fd, "%d", obj->d.integer32);
              break;

          case LLL_LAMBDA:     /* TODO: printing name. */
              fprintf(fd, "<lambda>");
              break;

          case LLL_BUILTIN_FUNCTION:
              fprintf(fd, "<builtin function>");
              break;

          default:
              printf("%x\n", obj->type_code);
              lll_fatal_error(6, "display", __FILE__, __LINE__);
        }
    }
}

void
lll_display(struct lll_object *obj) {
    lll_displayf(stdout, obj);
}
