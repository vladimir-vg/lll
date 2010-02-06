#!/bin/bash
# works with GNU indent 2.2.10
indent --line-length100                     \
       --no-tabs                            \
       --braces-on-if-line                  \
       --dont-cuddle-else                   \
       --cuddle-do-while                    \
       --case-indentation2                  \
       --dont-space-special-semicolon       \
       --no-space-after-parentheses         \
       --no-space-after-function-call-names \
       --blank-before-sizeof                \
       --space-after-for                    \
       --space-after-if                     \
       --space-after-while                  \
       --no-blank-lines-after-commas        \
       --dont-break-function-decl-args      \
       --dont-break-function-decl-args-end  \
       --procnames-start-lines              \
       --braces-on-struct-decl-line         \
       --braces-on-func-def-line            \
       --indent-level8                      \
       --continue-at-parentheses            \
       --parameter-indentation4             \
       --preprocessor-indentation1          \
       --indent-label0                      \
       --break-after-boolean-operator       \
       *.c *.h
rm *~